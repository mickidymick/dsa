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

/* #define PAGE_SIZE  4096 */
/* #define TIME_INIT  clock_t start; clock_t end; double cpu_time_used; */
/* #define TIME_START start = clock(); */
/* #define TIME_STOP  end = clock(); cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; printf("Time: %f seconds\n", cpu_time_used); */
/* #define PRINT_TH   printf("Throuput: %lld GB\\s\n", upper_size / cpu_time_used); */

/* typedef struct page { */
/*     char data[PAGE_SIZE]; */
/* } Page; */

int dsa_copy(void *src, void *dest, long long buffer_size);
int dsa_move(void *src, void *dest, long long buffer_size);
/* int dsa_mem(Page *src, Page *dest, long long buffer_size, long long arr_size); */

/* int main(int argc, char* argv[]) { */
/*     Page *mem_u; */
/*     Page *mem_l; */
/*     int upper_dev         = 0; */
/*     int lower_dev         = 1; */
/*     int upper_size_base_2 = 34; */
/*     long long upper_size  = pow(2, upper_size_base_2); */
/*     long long arr_size    = upper_size / PAGE_SIZE; */

/*     printf("Upper Size: %lld\n", upper_size); */

/*     TIME_INIT */

/*     printf("Allocating Upper Tier\n"); */
/*     mem_u = (Page *)numa_alloc_onnode(upper_size, 0); */
/*     if (mem_u == NULL) { */
/*         printf("Memory Allocation Failed!\n"); */
/*         return 1; */
/*     } */
/*     printf("Fault in Pages\n"); */
/*     memset(mem_u, 1, upper_size); */

/*     printf("Allocating Lower Tier\n"); */
/*     mem_l = (Page *)numa_alloc_onnode(upper_size, 1); */
/*     if (mem_l == NULL) { */
/*         printf("Memory Allocation Failed!\n"); */
/*         return 1; */
/*     } */
/*     printf("Fault in Pages\n\n"); */
/*     memset(mem_l, 2, upper_size); */

/*     printf("Calling DSA Copy\n"); */
/*     TIME_START */
/*     dsa_copy(mem_u, mem_l, upper_size); */
/*     memcpy(mem_u, mem_l, upper_size); */
/*     TIME_STOP */
/*     PRINT_TH */

/*     numa_free(mem_u, upper_size); */
/*     numa_free(mem_l, upper_size); */

/*     exit(0); */
/* } */

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

/*     uint32_t crc = 1; */

    dml_job_ptr->operation              = DML_OP_MEM_MOVE;
/*     dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY; */
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT;
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;
    dml_job_ptr->destination_length     = buffer_size;
/* //    dml_job_ptr->operation              = DML_OP_COPY_CRC; */
/* //    dml_job_ptr->crc_checksum_ptr       = &crc; */
/* //    dml_job_ptr->flags                  = DML_FLAG_CRC_READ_SEED; // read value from crc_checksum_ptr as crc seed */

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

int dsa_move(void *src, void *dest, long long buffer_size) {
/* //    dml_path_t execution_path = DML_PATH_SW; //software path */
    dml_path_t execution_path = DML_PATH_HW; //hardware path

    if (execution_path == DML_PATH_HW) {
        printf("Hardware Path\n");
    } else {
        printf("Softwareware Path\n");
    }

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
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;

/* //    for(int i = 0; i < arr_size; i++){ */
/* //        printf("%d -> %d\n", src->data[i], dest->data[i]); */
/* //    } */

    status = dml_execute_job(dml_job_ptr, DML_WAIT_MODE_BUSY_POLL);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job execution.\n", status);
        dml_finalize_job(dml_job_ptr);
        free(dml_job_ptr);
        return 1;
    }

/* //    for(int i = 0; i < arr_size; i++){ */
/* //        printf("%d -> %d\n", src->data[i], dest->data[i]); */
/* //    } */

/* //    for(int i = 0; i < arr_size; i++){ */
/* //        if(dest->data[i] != src->data[i]){ */
/* //            printf("Error: Operation result is incorrect.\n"); */
/* //            dml_finalize_job(dml_job_ptr); */
/* //            free(dml_job_ptr); */
/* //            return 1; */
/* //        } */
/* //    } */

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

/* MRJ -- TODO
 * Potential timing experiments:
 * -- Page faults with populate (still need to implement MAP_POPULATE)
 */

#define DRAM_NODEMASK (0x1ul)
#define PMEM_NODEMASK (0x2ul)
#define DRAM_NODE 0
#define PMEM_NODE 1
#define VMEM_PAGE_SIZE 4096
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
    region_size = 4096;

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

/*     vmem_mbind_region(region1, DRAM_NODEMASK); */
/*     vmem_mbind_region(region2, PMEM_NODEMASK); */

    r1_value = r2_value = r1_value_mig = r2_value_mig = 0;


printf("populating region1 (pre-fault) ... "); fflush(stdout);

    cur = (uint64_t) region1;
    end = ((uint64_t) region1 + region_size);

    start = getns();
/*     for (i = 0; cur < end; i++, cur += sizeof(uint64_t)) { */
/*         *((uint64_t*)cur) = (uint64_t)i; */
/*     } */
    memset((void *)cur, 0, region_size);
    dur = getns() - start;

    print_time_stats(dur, region_size);


printf("populating region2 (pre-fault) ... "); fflush(stdout);

    cur = (uint64_t) region2;
    end = ((uint64_t) region2 + region_size);

    start = getns();
/*     for (i = 0; cur < end; i++, cur += sizeof(uint64_t)) { */
/*         *((uint64_t*)cur) = (((uint64_t)i)>>1); */
/*     } */
    memset((void *)cur, 0, region_size);
    dur = getns() - start;

    print_time_stats(dur, region_size);


/* printf("accessing region1 (post-fault) ... "); fflush(stdout); */

/*     cur = (uint64_t) region1; */
/*     end = ((uint64_t) region1 + region_size); */

/*     start = getns(); */
/*     for (; cur < end; cur += sizeof(uint64_t)) { */
/*         v         = *((uint64_t*)cur); */
/*         r1_value += v; */
/*     } */
/*     dur = getns() - start; */

/*     print_time_stats(dur, region_size); */


/* printf("accessing region2 (post-fault) ... "); fflush(stdout); */

/*     cur = (uint64_t) region2; */
/*     end = ((uint64_t) region2 + region_size); */

/*     start = getns(); */
/*     for (; cur < end; cur += sizeof(uint64_t)) { */
/*         v         = *((uint64_t*)cur); */
/*         r2_value += v; */
/*     } */
/*     dur = getns() - start; */

/*     print_time_stats(dur, region_size); */


printf("migrating region1 down ........... "); fflush(stdout);

    start = getns();
/*     memcpy(region2, region1, region_size); */
/*     dsa_move(region1, region2, region_size); */
    dsa_copy(region1, region2, region_size);
/*     vmem_migrate_region_to_node(region1, PMEM_NODE); */
    dur = getns() - start;

    print_time_stats(dur, region_size);

/* printf("migrating region2 up ............. "); fflush(stdout); */

/*     start = getns(); */
/*     vmem_migrate_region_to_node(region2, DRAM_NODE); */
/*     dur = getns() - start; */

/*     print_time_stats(dur, region_size); */

/* printf("accessing region1 (post-mig) ..... "); fflush(stdout); */

/*     cur = (uint64_t) region1; */
/*     end = ((uint64_t) region1 + region_size); */

/*     start = getns(); */
/*     for (; cur < end; cur += sizeof(uint64_t)) { */
/*         v             = *((uint64_t*)cur); */
/*         r1_value_mig += v; */
/*     } */
/*     dur = getns() - start; */

/*     print_time_stats(dur, region_size); */


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


/*     if (r1_value != r1_value_mig) { */
/*         printf("region1 values do not match: pre_mig: %lu post_mig: %lu\n", */
/*                 r1_value, r1_value_mig); */

/*     } else { */
/*         printf("region1 value matches: %lu\n", r1_value); */
/*     } */

/*     if (r2_value != r2_value_mig) { */
/*         printf("region2 values do not match: pre_mig: %lu post_mig: %lu\n", */
/*                 r2_value, r2_value_mig); */

/*     } else { */
/*         printf("region2 value matches: %lu\n", r2_value); */
/*     } */

    if (r1_value != r2_value_mig) {
        printf("region1 values do not match: pre_mig: %lu post_mig: %lu\n",
                r1_value, r2_value_mig);

    } else {
        printf("region1 value matches: %lu\n", r1_value);
    }


    munmap(region1, region_size);
    munmap(region2, region_size);
}
