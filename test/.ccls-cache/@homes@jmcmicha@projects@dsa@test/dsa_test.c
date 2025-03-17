/* File:
 * Author: Zach McMichael
 * Description:
 */

#include "utils.h"
#include "dsa.h"

/* #define MEMCPY */
/* #define DSA_SYNC */
/* #define DSA_ASYNC */
#define DSA_BATCHED_ASYNC
/* #define DSA_THREADED_ASYNC */

int main(int argc, char *argv[]) {
    int       i;
    int       status;
    char     *c;
    void     *region1, *region2, *region3, *region4;
    void     *startup1, *startup2, *startup3, *startup4;
    ssize_t   region_size;
    uint64_t  cur, end, start, dur, mid;
    uint64_t  r1_value, r2_value_mig, r3_value, r4_value_mig;

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

    if (_dsa_init((int)DRAM_NODE, (int)PMEM_NODE) == 1) {
        return 1;
    }

#ifdef VERBOSE
    printf("Allocate Region1 ........................ "); fflush(stdout);
#endif
    region1 = allocate(region_size, (int)DRAM_NODE);

#ifdef VERBOSE
    printf("Allocate Region2 ........................ "); fflush(stdout);
#endif
    region2 = allocate(region_size, (int)PMEM_NODE);

#ifdef VERBOSE
    printf("Allocate Region3 ........................ "); fflush(stdout);
#endif
    region3 = allocate(region_size, (int)PMEM_NODE);

#ifdef VERBOSE
    printf("Allocate Region4 ........................ "); fflush(stdout);
#endif
    region4 = allocate(region_size, (int)DRAM_NODE);

#ifdef VERBOSE
    printf("populating region1 (pre-fault) .......... "); fflush(stdout);
#endif
    populate_region(region1, region_size, 1);

#ifdef VERBOSE
    printf("populating region2 (pre-fault) .......... "); fflush(stdout);
#endif
    populate_region(region2, region_size, 0);

#ifdef VERBOSE
    printf("populating region3 (pre-fault) .......... "); fflush(stdout);
#endif
    populate_region(region3, region_size, 1);

#ifdef VERBOSE
    printf("populating region4 (pre-fault) .......... "); fflush(stdout);
#endif
    populate_region(region4, region_size, 0);

#ifdef VERBOSE
    printf("accessing region1 (post-fault) .......... "); fflush(stdout);
#endif
    r1_value = access_region(region1, region_size);

#ifdef VERBOSE
    printf("accessing region3 (post-fault) .......... "); fflush(stdout);
#endif
    r3_value = access_region(region3, region_size);


/*     if (_dsa_init((int)DRAM_NODE, (int)PMEM_NODE) == 1) { */
/*         return 1; */
/*     } */
/*     int startup_size = VMEM_PAGE_SIZE * 2; */
/*     int startup_size = region_size; */
/* #ifdef VERBOSE */
/*     printf("Allocate Startup1 ....................... "); fflush(stdout); */
/* #endif */
/*     startup1 = allocate(startup_size, (int)DRAM_NODE); */
/* #ifdef VERBOSE */
/*     printf("Allocate Startup2 ....................... "); fflush(stdout); */
/* #endif */
/*     startup2 = allocate(startup_size, (int)PMEM_NODE); */
/* #ifdef VERBOSE */
/*     printf("populating Startup1 (pre-fault) ......... "); fflush(stdout); */
/* #endif */
/*     populate_region(startup1, startup_size, 1); */
/* #ifdef VERBOSE */
/*     printf("populating Startup2 (pre-fault) ......... "); fflush(stdout); */
/* #endif */
/*     populate_region(startup2, startup_size, 0); */
/*     status = dsa_sync_copy(startup2, startup1, startup_size, VMEM_PAGE_SIZE); */
/*     status = dsa_batched_async_copy(startup2, startup1, startup_size, VMEM_PAGE_SIZE); */
/*     if (status == 1) { */
/*         return 1; */
/*     } */
/* #ifdef VERBOSE */
/*     printf("Allocate Startup3 ....................... "); fflush(stdout); */
/* #endif */
/*     startup3 = allocate(startup_size, (int)PMEM_NODE); */
/* #ifdef VERBOSE */
/*     printf("Allocate Startup4 ....................... "); fflush(stdout); */
/* #endif */
/*     startup4 = allocate(startup_size, (int)DRAM_NODE); */
/* #ifdef VERBOSE */
/*     printf("populating Startup3 (pre-fault) ......... "); fflush(stdout); */
/* #endif */
/*     populate_region(startup3, startup_size, 1); */
/* #ifdef VERBOSE */
/*     printf("populating Startup4 (pre-fault) ......... "); fflush(stdout); */
/* #endif */
/*     populate_region(startup4, startup_size, 0); */
/*     status = dsa_sync_copy(startup2, startup1, startup_size, VMEM_PAGE_SIZE); */
/*     status = dsa_batched_async_copy(startup4, startup3, startup_size, VMEM_PAGE_SIZE); */
/*     if (status == 1) { */
/*         return 1; */
/*     } */



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

#ifdef MEMCPY
#ifdef VERBOSE
    printf("migrating region3 up   memcpy ........... "); fflush(stdout);
#endif
    start = getns();

    status = cpu_copy(region4, region3, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_SYNC
#ifdef VERBOSE
    printf("migrating region3 up   dsa_sync ......... "); fflush(stdout);
#endif
    start = getns();

    status = dsa_sync_copy(region4, region3, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_ASYNC
#ifdef VERBOSE
    printf("migrating region3 up   dsa_async ........ "); fflush(stdout);
#endif
    start = getns();

    status = dsa_async_copy(region4, region3, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_BATCHED_ASYNC
#ifdef VERBOSE
    printf("migrating region3 up   dsa_batched_async. "); fflush(stdout);
#endif
    start = getns();

    status = dsa_batched_async_copy(region4, region3, region_size, VMEM_PAGE_SIZE);
#endif

#ifdef DSA_THREADED_ASYNC
#ifdef VERBOSE
    printf("migrating region3 up   dsa_threaded_async "); fflush(stdout);
#endif
    start = getns();

    status = dsa_threaded_async_copy(region4, region3, region_size, VMEM_PAGE_SIZE);
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

#ifdef VERBOSE
    printf("accessing region4 (post-mig) ............ "); fflush(stdout);
#endif
    r4_value_mig = access_region(region4, region_size);

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

    if (r3_value != r4_value_mig) {
#ifdef VERBOSE
        printf("region3 value do not match region4: pre_mig: %lu post_mig: %lu\n",
                r3_value, r4_value_mig);
#endif

    } else {
#ifdef VERBOSE
        printf("region3 value matches region4: %lu\n", r4_value_mig);
#endif
    }

    munmap(region1, region_size);
    munmap(region2, region_size);
    munmap(region3, region_size);
    munmap(region4, region_size);
}
