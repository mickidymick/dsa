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
    if (status != DML_STATUS_OK) {
        printf("FUCK!\n");
        printf("An error (%u) occurred during getting job size.\n", status);
    }

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

void print_time_stats(uint64_t dur, ssize_t region_size) {
    ssize_t nr_vpages, nr_hpages;
    double  seconds, region_GBs;

    region_GBs = (region_size / (1024.0 * 1024.0 * 1024.0));
    seconds    = ((double)(dur) / (1000.0 * 1000.0 * 1000.0));
    nr_vpages  = (region_size / VMEM_PAGE_SIZE);
    nr_hpages  = (region_size / VMEM_H_PAGE_SIZE);

printf("done. elapsed us: %12.2f    per-vpage us: %10.2f    per-hpage us: %10.2f    GB/s: %10.2f\n",
        ns2us(dur), (ns2us(dur) / nr_vpages), (ns2us(dur) / nr_hpages),
        (region_GBs / seconds));
}

void *init(long long num_pages) {
    void *region;
    uint64_t cur, end, v, ret;
    char *str;

    str = (char *)malloc(1024);
    ssize_t region_size = VMEM_PAGE_SIZE * num_pages;
    bytes2str(region_size, str);
    printf("init regions: num_pages:%lld tot_size:%s\n", num_pages, str);

    region = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region == MAP_FAILED) {
        perror ("r1 mmap failed");
        exit(EXIT_FAILURE);
    }

    return region;
}

int dml_check(dml_job_t *job_ptr) {
    dml_status_t status = dml_check_job(job_ptr);
    if (status == DML_STATUS_OK) {
        return 1;
    } else if (status == DML_STATUS_BEING_PROCESSED) {
        return 0;
    } else {
        return -1;
    }
}

int async_loop(int max_jobs, int num_pages, void *r1, void *r2) {
    int dml_arr_max_size = max_jobs;
    int dml_arr_size = 0;
    dml_job_t *tmp_dml_job_ptr;
    dml_job_t *job_arr[dml_arr_max_size];
    for (int i = 0; i < dml_arr_max_size; i++) {
        job_arr[i] = NULL;
    }

    //loop over region page by page
    for (long long i = 0; i < num_pages; i++) {
/*         printf("page:%lld total:%d\n", i, num_pages); */

        //if we can submit a new job
        if (dml_arr_size < dml_arr_max_size) {
do_copy:
/*             printf("start copy\n"); */
            tmp_dml_job_ptr = dsa_copy_start(r1 + (i * VMEM_PAGE_SIZE), r2 + (i * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE);
/*             printf("job_ptr:%p\n", tmp_dml_job_ptr); */
            if (tmp_dml_job_ptr == NULL) {
                printf("ERROR!\n");
                return 1;
            }

            //add job to arr
            for (int i = 0; i < dml_arr_max_size; i++) {
                if (job_arr[i] == NULL) {
                    job_arr[i] = tmp_dml_job_ptr;
                    dml_arr_size += 1;
/*                     printf("added job\n"); */
                    break;
                }
            }

        } else {
            while (dml_arr_size > 0) {
/*                 printf("inside\n"); */
/*                 printf("arr_size:%d\n", dml_arr_size); */
/*                 printf("looking for completed job\n"); */
                for (int i = 0; i < dml_arr_max_size; i++) {
                    int status = dml_check(job_arr[i]);
                    //job completed
                    if (status == 1) {
/*                         printf("job completed\n"); */
                        free(job_arr[i]);
                        job_arr[i] = NULL;
                        dml_arr_size -= 1;
                        goto do_copy;
                    //job errored
                    } else if (status != 0){
                        printf("An error (%u) occurred during job finalization.\n", dml_check_job(job_arr[i]));
                        return 1;
                    }
                }
/*                 printf("waiting\n"); */
            }
        }
    }

    while (dml_arr_size > 0) {
/*         printf("arr_size:%d\n", dml_arr_size); */
/*         printf("looking for completed job\n"); */
        for (int i = 0; i < dml_arr_max_size; i++) {
/*             printf("job_arr[%d]: %p\n", i, job_arr[i]); */
            if (job_arr[i] != NULL) {
                int status = dml_check(job_arr[i]);
                //job completed
                if (status == 1) {
/*                     printf("job completed\n"); */
                    free(job_arr[i]);
                    job_arr[i] = NULL;
                    dml_arr_size -= 1;
                //job errored
                } else if (status != 0){
                    printf("An error (%u) occurred during job finalization.\n", dml_check_job(job_arr[i]));
                    return 1;
                }
            }
        }
/*         printf("waiting\n"); */
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int       i;
    void     *region1, *region2;
    void     *region3, *region4;
    void     *region5, *region6;
    void     *region7, *region8;
    ssize_t   region_size;
    uint64_t  cur, end1, end2, end3, end4, start1, start2, start3, start4, dur;
    uint64_t  v, r1_value, r2_value, r1_value_mig, r2_value_mig;

    long long num_pages = 4;

    region_size = num_pages * VMEM_PAGE_SIZE;

    printf("init regions\n");

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
    memset((void *)region1, 1, region_size);
    memset((void *)region2, 2, region_size);

    region3 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region3 == MAP_FAILED) {
        perror ("region3 mmap failed");
        exit(EXIT_FAILURE);
    }
    region4 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region4 == MAP_FAILED) {
        perror ("region4 mmap failed");
        exit(EXIT_FAILURE);
    }
    mbind(region3, region_size, MPOL_BIND, (const unsigned long *)DRAM_NODEMASK, 3, 0);
    mbind(region4, region_size, MPOL_BIND, (const unsigned long *)PMEM_NODEMASK, 3, 0);
    memset((void *)region3, 1, region_size);
    memset((void *)region4, 2, region_size);

    region5 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region5 == MAP_FAILED) {
        perror ("region5 mmap failed");
        exit(EXIT_FAILURE);
    }
    region6 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region6 == MAP_FAILED) {
        perror ("region6 mmap failed");
        exit(EXIT_FAILURE);
    }
    mbind(region5, region_size, MPOL_BIND, (const unsigned long *)DRAM_NODEMASK, 3, 0);
    mbind(region6, region_size, MPOL_BIND, (const unsigned long *)PMEM_NODEMASK, 3, 0);
    memset((void *)region5, 1, region_size);
    memset((void *)region6, 2, region_size);

    region7 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region7 == MAP_FAILED) {
        perror ("region7 mmap failed");
        exit(EXIT_FAILURE);
    }
    region8 = mmap(NULL, region_size, (PROT_READ | PROT_WRITE),
                    (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0);
    if (region8 == MAP_FAILED) {
        perror ("region8 mmap failed");
        exit(EXIT_FAILURE);
    }
    mbind(region7, region_size, MPOL_BIND, (const unsigned long *)DRAM_NODEMASK, 3, 0);
    mbind(region8, region_size, MPOL_BIND, (const unsigned long *)PMEM_NODEMASK, 3, 0);
    memset((void *)region7, 1, region_size);
    memset((void *)region8, 2, region_size);

    printf("migrating region1 down ........... \n"); fflush(stdout);

    int size = 4;
    int dml_arr_size = size;
    dml_job_t *tmp_dml_job_ptr;
    dml_job_t *job_arr[size];
    for (int i = 0; i < size; i++) {
        job_arr[i] = NULL;
    }
    job_arr[0] = dsa_copy_start(region1, region2, region_size);
    start1 = getns();
    job_arr[1] = dsa_copy_start(region3, region4, region_size);
    start2 = getns();
    job_arr[2] = dsa_copy_start(region5, region6, region_size);
    start3 = getns();
    job_arr[3] = dsa_copy_start(region7, region8, region_size);
    start4 = getns();

    while (dml_arr_size > 0) {
        for (int i = 0; i < size; i++) {
            if (job_arr[i] != NULL) {
                int status = dml_check(job_arr[i]);
                if (status == 1) {
                    if (i == 0) {
                        end1 = getns();
                    } else if (i == 1) {
                        end2 = getns();
                    } else if (i == 2) {
                        end3 = getns();
                    } else if (i == 3) {
                        end4 = getns();
                    }
                    free(job_arr[i]);
                    job_arr[i] = NULL;
                    dml_arr_size -= 1;
                } else if (status != 0){
                    printf("An error (%u) occurred during job finalization.\n", dml_check_job(job_arr[i]));
                    return 1;
                }
            }
        }
    }
    print_time_stats(end1-start1, region_size);
    print_time_stats(end2-start2, region_size);
    print_time_stats(end3-start3, region_size);
    print_time_stats(end4-start4, region_size);

    munmap(region1, region_size);
    munmap(region2, region_size);
    munmap(region3, region_size);
    munmap(region4, region_size);
    munmap(region5, region_size);
    munmap(region6, region_size);
    munmap(region7, region_size);
    munmap(region8, region_size);
}
