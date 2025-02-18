/* File:
 * Author: Zach McMichael
 * Description:
 */

#include "utils.h"
#include "dsa.h"

/* #define MEMCPY */
/* #define DSA_SYNC */
#define DSA_ASYNC
/* #define DSA_BATCHED_ASYNC */
/* #define DSA_THREADED_ASYNC */

int main(int argc, char *argv[]) {
    int       i;
    int       status;
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
    status = 0;

#ifdef VERBOSE
    printf("DRAM Node:         %d\n", DRAM_NODE);
    printf("PMEM NODE:         %d\n", PMEM_NODE);
#endif

    c = malloc(sizeof(char) * 1024);
    bytes2str(VMEM_PAGE_SIZE, c);
#ifdef VERBOSE
    printf("Transfer Size:     %s\n", c);
#endif

    bytes2str(region_size, c);
#ifdef VERBOSE
    printf("Total Region Size: %s\n", c);
#endif
    free(c);

#ifdef VERBOSE
    printf("Allocate Region1 ........................ "); fflush(stdout);
#endif
    region1 = allocate(region_size, (int)DRAM_NODE);

#ifdef VERBOSE
    printf("Allocate Region2 ........................ "); fflush(stdout);
#endif
    region2 = allocate(region_size, (int)PMEM_NODE);

#ifdef VERBOSE
    printf("populating region1 (pre-fault) .......... "); fflush(stdout);
#endif
    populate_region(region1, region_size, 1);

#ifdef VERBOSE
    printf("populating region2 (pre-fault) .......... "); fflush(stdout);
#endif
    populate_region(region2, region_size, 0);

#ifdef VERBOSE
    printf("accessing region1 (post-fault) .......... "); fflush(stdout);
#endif
    r1_value = access_region(region1, region_size);

#ifdef MEMCPY
#ifdef VERBOSE
    printf("migrating region1 down memcpy ........... "); fflush(stdout);
#endif
    start = getns();

    status = cpu_copy(region2, region1, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_SYNC
#ifdef VERBOSE
    printf("migrating region1 down dsa_sync ......... "); fflush(stdout);
#endif
    start = getns();

    status = dsa_sync_copy(region2, region1, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_ASYNC
#ifdef VERBOSE
    printf("migrating region1 down dsa_async ........ "); fflush(stdout);
#endif
    start = getns();

    status = dsa_async_copy(region2, region1, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_BATCHED_ASYNC
#ifdef VERBOSE
    printf("migrating region1 down dsa_batched_async. "); fflush(stdout);
#endif
    start = getns();

    status = dsa_batched_async_copy(region2, region1, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_THREADED_ASYNC
#ifdef VERBOSE
    printf("migrating region1 down dsa_threaded_async "); fflush(stdout);
#endif
    start = getns();

    status = dsa_threaded_async_copy(region2, region1, region_size, VMEM_PAGE_SIZE);
#endif

    if (status == 1) {
        return 1;
    }

    dur = getns() - start;
    print_time_stats(dur, region_size, 1);

#ifdef VERBOSE
    printf("accessing region2 (post-mig) ............ "); fflush(stdout);
#endif
    r2_value_mig = access_region(region2, region_size);

    if (r1_value != r2_value_mig) {
#ifdef VERBOSE
        printf("region1 value do not match region2: pre_mig: %lu post_mig: %lu\n",
                r1_value, r2_value_mig);
#endif

    } else {
#ifdef VERBOSE
        printf("region1 value matches region2: %lu\n", r2_value_mig);
#endif
    }

    munmap(region1, region_size);
    munmap(region2, region_size);
}
