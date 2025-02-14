/* File:
 * Author: Zach McMichael
 * Description:
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <numa.h>
#include <numaif.h>
#include <time.h>
#include <sys/mman.h>
#include "dml.h"

/* typedef struct page { */
/*     char data[PAGE_SIZE]; */
/* } Page; */

int dsa_copy(void *src, void *dest, long long buffer_size);
dml_job_t *dsa_copy_start(void *src, void *dest, long long buffer_size);
int dsa_copy_end(dml_job_t *dml_job_ptr);

int dsa_copy(void *src, void *dest, long long buffer_size) {
    dml_path_t execution_path = DML_PATH_HW; //hardware path
    dml_job_t *dml_job_ptr = NULL;
    uint32_t size = 0u;

    dml_status_t status = dml_get_job_size(execution_path, &size);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during getting job size.\n", status);
        return 1;
    }

    dml_job_ptr = (dml_job_t *)malloc(size);

    status = dml_init_job(execution_path, dml_job_ptr);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job initialization.\n", status);
        free(dml_job_ptr);
        return 1;
    }

    dml_job_ptr->operation              = DML_OP_MEM_MOVE;
/*     dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY; */
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT;
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;
    dml_job_ptr->destination_length     = buffer_size;

    status = dml_execute_job(dml_job_ptr, DML_WAIT_MODE_BUSY_POLL);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job execution.\n", status);
        dml_finalize_job(dml_job_ptr);
        free(dml_job_ptr);
        return 1;
    }

    status = dml_finalize_job(dml_job_ptr);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        free(dml_job_ptr);
        return 1;
    }
    free(dml_job_ptr);

/*     printf("Job Completed Successfully.\n"); */
    return 0;
}

dml_job_t *dsa_copy_start(void *src, void *dest, long long buffer_size) {
    dml_path_t execution_path = DML_PATH_HW; //hardware path
    dml_job_t *dml_job_ptr = NULL;
    uint32_t size = 0u;

    dml_status_t status = dml_get_job_size(execution_path, &size);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during getting job size.\n", status);
        return NULL;
    }

    dml_job_ptr = (dml_job_t *)malloc(size);

    status = dml_init_job(execution_path, dml_job_ptr);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job initialization.\n", status);
        free(dml_job_ptr);
        return NULL;
    }

    dml_job_ptr->operation              = DML_OP_MEM_MOVE;
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY;
/*     dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT; */
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;
    dml_job_ptr->destination_length     = buffer_size;

    status = dml_submit_job(dml_job_ptr);

    //printf("Job Started Successfully.\n");
    return dml_job_ptr;
}

int dsa_copy_end(dml_job_t *dml_job_ptr) {
    dml_status_t status = dml_finalize_job(dml_job_ptr);

    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        free(dml_job_ptr);
        return 1;
    }

    free(dml_job_ptr);

    return 0;
}

#define DRAM_NODEMASK (0x1ul)
#define PMEM_NODEMASK (0x2ul)
#define DRAM_NODE 0
#define PMEM_NODE 1
#define VMEM_PAGE_SIZE 2097152
/* #define VMEM_PAGE_SIZE 4096 */
#define VMEM_H_PAGE_SIZE 4096

uint64_t getns(void) {
    struct timespec ts;
    int             ret;

    ret = clock_gettime(CLOCK_MONOTONIC, &ts);

    return (((uint64_t)ts.tv_sec) * 1000000000ULL) + ts.tv_nsec;
}

double ns2us(uint64_t ns) {
    return ((double)ns/1000.0);
}

double ns2ms(uint64_t ns) {
    return ((double)ns/1000000.0);
}

void bytes2str(size_t bytes, char *str) {
    if (bytes < (1ULL<<10)) {
        sprintf(str,"%ldB", bytes);

    } else if (bytes < (1ULL<<20)) {
        sprintf(str,"%ldK", (bytes/(1024L)));

    } else if (bytes <(1ULL<<30)) {
        sprintf(str,"%ldM", (bytes/(1024L*1024L)));

    } else if (bytes < (1ULL<<40)) {
        sprintf(str,"%ldG", (bytes/(1024L*1024L*1024L)));
    }
}

void print_time_stats(uint64_t dur, ssize_t region_size, int color) {
    ssize_t nr_vpages, nr_hpages;
    double  seconds, region_GBs;

    region_GBs = (region_size / (1024.0 * 1024.0 * 1024.0));
    seconds    = ((double)(dur) / (1000.0 * 1000.0 * 1000.0));
    nr_vpages  = (region_size / VMEM_PAGE_SIZE);
    nr_hpages  = (region_size / VMEM_H_PAGE_SIZE);

    if (color == 1) {
        printf("done. elapsed us: %12.2f    per-vpage us: %10.2f    per-hpage us: %10.2f    \033[0;32m GB/s: %10.2f\n \033[0;32m",
            ns2us(dur), (ns2us(dur) / nr_vpages), (ns2us(dur) / nr_hpages), (region_GBs / seconds));
    } else {
        printf("done. elapsed us: %12.2f    per-vpage us: %10.2f    per-hpage us: %10.2f    GB/s: %10.2f\n",
            ns2us(dur), (ns2us(dur) / nr_vpages), (ns2us(dur) / nr_hpages), (region_GBs / seconds));
    }
}

int main(int argc, char *argv[]) {
    int       i;
    void     *region1, *region2;
    ssize_t   region_size;
    uint64_t  cur, end, start, dur;
    uint64_t  v, r1_value, r2_value, r1_value_mig, r2_value_mig;

    if (argc < 2) {
        fprintf(stderr, "%s region_size(MBs)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    region_size = atoll(argv[1]) * (1024L * 1024L);
    char *tmp_str;
    tmp_str = (char *)malloc(1024);
    bytes2str(region_size, tmp_str);
    printf("region size: %s\n", tmp_str);

    region1 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region1 == MAP_FAILED) {
        perror ("region1 mmap failed");
        exit(EXIT_FAILURE);
    }

    region2 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region2 == MAP_FAILED) {
        perror ("region2 mmap failed");
        exit(EXIT_FAILURE);
    }

    mbind(region1, region_size, MPOL_BIND, (const unsigned long *)DRAM_NODEMASK, 3, 0);
    mbind(region2, region_size, MPOL_BIND, (const unsigned long *)PMEM_NODEMASK, 3, 0);

    r1_value = r2_value = r1_value_mig = r2_value_mig = 0;


printf("populating region1 (pre-fault) ... "); fflush(stdout);

    cur = (uint64_t) region1;
    end = ((uint64_t) region1 + region_size);

    start = getns();
    memset((void *)cur, 1, region_size);
    dur = getns() - start;

    print_time_stats(dur, region_size);


printf("populating region2 (pre-fault) ... "); fflush(stdout);

    cur = (uint64_t) region2;
    end = ((uint64_t) region2 + region_size);

    start = getns();
    memset((void *)cur, 0, region_size);
    dur = getns() - start;

    print_time_stats(dur, region_size);

printf("accessing region1 (post-fault) ... "); fflush(stdout);

    cur = (uint64_t) region1;
    end = ((uint64_t) region1 + region_size);

    start = getns();
    for (; cur < end; cur += sizeof(uint64_t)) {
        v         = *((uint64_t*)cur);
        r1_value += v;
    }
    dur = getns() - start;

    print_time_stats(dur, region_size);

printf("migrating region1 down ........... \n"); fflush(stdout);

    int len = region_size / VMEM_PAGE_SIZE;
    printf("region_size: %lu\n", region_size);
    printf("vmem_page_size: %d\n", VMEM_PAGE_SIZE);
    printf("len: %d\n", len);

    start = getns();
    //separate by vmem_page_size

    //whole region
/*     dsa_copy(region1, region2, region_size); */

    //page by page
/*     for (long long i; i < len; i++) { */
/*         dsa_copy(region1 + (i * VMEM_PAGE_SIZE), region2 + (i * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE); */
/*     } */

///*
#define MAX_SIZE 16
    int dml_arr_max_size = MAX_SIZE;
    int dml_arr_size = 0;
    dml_job_t *tmp_dml_job_ptr;
    dml_job_t *job_arr[MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; i++) {
        job_arr[i] = NULL;
    }

    //loop over all pages in region
    for (long long i; i < len; i++) {
/*         printf("i:%d len:%d\n", i, len); */
        //if we can submit a new job
        //printf("size: %d max:%d\n", dml_arr_size, dml_arr_max_size);
        if (dml_arr_size < dml_arr_max_size) {
do_copy:
            //printf("do copy\n");
            tmp_dml_job_ptr = dsa_copy_start(region1 + (i * VMEM_PAGE_SIZE), region2 + (i * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE);
            if (tmp_dml_job_ptr == NULL) {
                printf("ERROR!\n");
                return 0;
            }

            //add job to arr
            for (int i = 0; i < dml_arr_max_size; i++) {
                if (job_arr[i] == NULL) {
                    job_arr[i] = tmp_dml_job_ptr;
                    dml_arr_size += 1;
                    break;
                }
            }
        } else {
            while (1) {
                //printf("waiting!\n");
                for (int i = 0; i < dml_arr_size; i++) {
                    if (dml_check_job(job_arr[i]) == DML_STATUS_OK) {
                        printf("job completed\n");
                        free(job_arr[i]);
                        job_arr[i] = NULL;
                        dml_arr_size -= 1;
                        goto do_copy;
                    }
                }
                //sleep(1);
            }
        }
    }
//*/
    dur = getns() - start;

    print_time_stats(dur, region_size);

printf("accessing region2 (post-mig) ..... "); fflush(stdout);

    cur = (uint64_t) region2;
    end = ((uint64_t) region2 + region_size);

    start = getns();
    for (; cur < end; cur += sizeof(uint64_t)) {
        v             = *((uint64_t*)cur);
        r2_value_mig += v;
    }
    dur = getns() - start;

    print_time_stats(dur, region_size);

    if (r1_value != r2_value_mig) {
        printf("region1 values do not match: pre_mig: %lu post_mig: %lu\n",
                r1_value, r2_value_mig);

    } else {
        printf("region1 value matches: %lu\n", r1_value);
    }

    munmap(region1, region_size);
    munmap(region2, region_size);
}
