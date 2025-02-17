/* File:
 * Author: Zach McMichael
 * Description:
 */

#include "utils.h"
#include "dsa.h"

#define MEMCPY
/* #define DSA_SYNC */
/* #define DSA_THREADED_SYNC */
/* #define DSA_BATCHED_SYNC */
/* #define DSA_ASYNC */
/* #define DSA_ASYNC_LOOP */
/* #define DSA_BATCHED_ASYNC */
/* #define DSA_THREADED_ASYNC */
/* #define DSA_BATCH */

int main(int argc, char *argv[]) {
    int       i;
    char     *c;
    void     *region1, *region2;
    ssize_t   region_size;
    uint64_t  cur, end, start, dur, mid;
    uint64_t  r1_value, r2_value_mig;

    if (argc < 2) {
        fprintf(stderr, "%s region_size(MBs)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    region_size = atoll(argv[1]) * (1024L * 1024L);
/*     region_size = 126976; */
    r1_value = r2_value_mig = 0;

    printf("DRAM Node:         %d\n", DRAM_NODE);
    printf("PMEM NODE:         %d\n", PMEM_NODE);

    c = malloc(sizeof(char) * 1024);
    bytes2str(VMEM_PAGE_SIZE, c);
    printf("Transfer Size:     %s\n", c);
    bytes2str(region_size, c);
    printf("Total Region Size: %s\n", c);
    free(c);

    printf("Allocate Region1 ........................ "); fflush(stdout);
    region1 = allocate(region_size, (int)DRAM_NODE);
/*     region1 = allocate(region_size, (const unsigned long *)DRAM_NODEMASK, (unsigned long)MAX_NODEMASK); */

    printf("Allocate Region2 ........................ "); fflush(stdout);
    region2 = allocate(region_size, (int)PMEM_NODE);
/*     region2 = allocate(region_size, (const unsigned long *)PMEM_NODEMASK, (unsigned long)MAX_NODEMASK); */

    printf("populating region1 (pre-fault) .......... "); fflush(stdout);
    populate_region(region1, region_size, 1);

    printf("populating region2 (pre-fault) .......... "); fflush(stdout);
    populate_region(region2, region_size, 0);

    printf("accessing region1 (post-fault) .......... "); fflush(stdout);
    r1_value = access_region(region1, region_size);

#ifdef MEMCPY
    printf("migrating region1 down memcpy ........... "); fflush(stdout);
    start = getns();

    for( uint64_t i = 0; i < region_size; i += VMEM_PAGE_SIZE) {
        memcpy(region2 + i, region1 + i, VMEM_PAGE_SIZE);
    }
#endif

#ifdef DSA_SYNC
    printf("migrating region1 down dsa_sync ......... "); fflush(stdout);
    start = getns();

    for( uint64_t i = 0; i < region_size; i += VMEM_PAGE_SIZE) {
        dsa_copy(region2 + i, region1 + i, VMEM_PAGE_SIZE);
    }
#endif

#ifdef DSA_THREADED_SYNC
    printf("migrating region1 down dsa_threaded_sync. "); fflush(stdout);
    start = getns();

    dsa_threaded_sync_copy(region2, region1, region_size);
#endif

#ifdef DSA_BATCHED_SYNC
    printf("Not Written\n");
/*     printf("migrating region1 down dsa_batched_sync.. "); fflush(stdout); */
/*     start = getns(); */

/*     dsa_batched_sync_copy(region2, region1, region_size); */
#endif

#ifdef DSA_ASYNC
    printf("migrating region1 down dsa_async ........ "); fflush(stdout);
    start = getns();

    uint64_t num_jobs_running = 0;
    uint64_t dml_arr_size = (uint64_t)region_size / (uint64_t)VMEM_PAGE_SIZE;
    dml_job_t **job_arr = malloc(sizeof(dml_job_t *) * dml_arr_size);
/*     for (uint64_t i = 0; i < dml_arr_size; i++) { */
/*         job_arr[i] = NULL; */
/*     } */
    memset(job_arr, 0, sizeof(dml_job_t *) * dml_arr_size);

    uint64_t k = 0;
    for( uint64_t i = 0; i < region_size; i += VMEM_PAGE_SIZE) {
        job_arr[k] = dsa_async_copy_start(region2 + i, region1 + i, VMEM_PAGE_SIZE, i % 4);
        num_jobs_running++;
        k++;
    }

/*     printf("num_jobs:%lu\n", num_jobs_running); */

    while (num_jobs_running > 0) {
        for (uint64_t i = 0; i < dml_arr_size; i++) {
            if (job_arr[i] != NULL && dml_check(job_arr[i]) == 1) {
                dsa_async_copy_end(job_arr[i]);
                job_arr[i] = NULL;
                num_jobs_running--;
            }
        }
    }

    free(job_arr);
#endif

#ifdef DSA_ASYNC_LOOP
    printf("migrating region1 down dsa_async_loop.... "); fflush(stdout);
    start = getns();

    dsa_loop_async_copy(region2, region1, region_size);
#endif

#ifdef DSA_BATCHED_ASYNC
    printf("migrating region1 down dsa_batched_async. "); fflush(stdout);
    start = getns();

    uint64_t num_jobs_running = 0;
    uint64_t num_batches      = (uint64_t)region_size / (uint64_t)VMEM_PAGE_SIZE / MAX_BATCH_SIZE;
/*     printf("num_batches: %lu\n", num_batches); */

    uint64_t remainder        = ((uint64_t)region_size / (uint64_t)VMEM_PAGE_SIZE) % MAX_BATCH_SIZE;
/*     printf("remainder: %lu\n", remainder); */

    if (remainder > 0) {
        num_batches++;
    }

    uint64_t size = (uint64_t)VMEM_PAGE_SIZE * MAX_BATCH_SIZE;
/*     printf("size: %lu\n", size); */

/*     uint64_t remainder_size = (uint64_t)VMEM_PAGE_SIZE *  remainder; */
/*     printf("remainder size: %lu\n", remainder_size); */

    uint64_t dml_arr_size     = num_batches;
    dml_job_t **job_arr       = malloc(sizeof(dml_job_t *) * dml_arr_size);

    for (uint64_t i = 0; i < dml_arr_size; i++) {
        job_arr[i] = NULL;
    }

    uint64_t k = 0;
    for( uint64_t i = 0; i < region_size; i += size) {
        if (remainder > 0 && k == num_batches - 1) {
            job_arr[k] = dsa_async_batch_copy_start(region2 + i, region1 + i, VMEM_PAGE_SIZE, remainder, i % 4);
        } else {
            job_arr[k] = dsa_async_batch_copy_start(region2 + i, region1 + i, VMEM_PAGE_SIZE, MAX_BATCH_SIZE, i % 4);
        }
        num_jobs_running++;
        k++;
    }

/*     printf("num_jobs:%lu\n", num_jobs_running); */

    while (num_jobs_running > 0) {
        for (uint64_t i = 0; i < dml_arr_size; i++) {
            if (job_arr[i] != NULL && dml_check(job_arr[i]) == 1) {
                dsa_async_batch_copy_end(job_arr[i]);
                job_arr[i] = NULL;
                num_jobs_running--;
            }
        }
    }

    free(job_arr);
#endif

#ifdef DSA_THREADED_ASYNC
    printf("migrating region1 down dsa_threaded_async "); fflush(stdout);
    start = getns();

    dsa_threaded_async_copy(region2, region1, region_size);
#endif

    dur = getns() - start;
    print_time_stats(dur, region_size, 1);

    printf("accessing region2 (post-mig) ............ "); fflush(stdout);
    r2_value_mig = access_region(region2, region_size);

    if (r1_value != r2_value_mig) {
        printf("region1 value do not match region2: pre_mig: %lu post_mig: %lu\n",
                r1_value, r2_value_mig);

    } else {
        printf("region1 value matches region2: %lu\n", r2_value_mig);
    }

    munmap(region1, region_size);
    munmap(region2, region_size);
}
