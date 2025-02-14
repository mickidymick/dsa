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
/* #define DSA_ASYNC_LOOP */
/* #define DSA_THREADED_ASYNC */

int main(int argc, char *argv[]) {
    int       i;
    void     *region1, *region2;
    ssize_t   region_size;
    uint64_t  cur, end, start, dur;
    uint64_t  r1_value, r2_value_mig;

    if (argc < 2) {
        fprintf(stderr, "%s region_size(MBs)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    region_size = atoll(argv[1]) * (1024L * 1024L);
    r1_value = r2_value_mig = 0;

    printf("DRAM Node:         %d\n", DRAM_NODE);
    printf("PMEM NODE:         %d\n", PMEM_NODE);
    printf("Transfer Size:     %d\n", VMEM_PAGE_SIZE);
    print_size(region_size);
    printf("\n");

    printf("Allocate Region1 ........................ "); fflush(stdout);
    region1 = allocate(region_size, (const unsigned long *)DRAM_NODEMASK);

    printf("Allocate Region2 ........................ "); fflush(stdout);
    region2 = allocate(region_size, (const unsigned long *)PMEM_NODEMASK);

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
    printf("migrating region1 down dsa_batched_sync.. "); fflush(stdout);
    start = getns();

    dsa_batched_sync_copy(region2, region1, region_size);
#endif

#ifdef DSA_ASYNC_LOOP
    printf("migrating region1 down dsa_async_loop.... "); fflush(stdout);
    start = getns();

    dsa_loop_async_copy(region2, region1, region_size);
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
