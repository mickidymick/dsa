#include "utils.h"

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
        printf("done. elapsed us: %12.2f    per-vpage us: %10.2f    per-hpage us: %10.2f    GB/s:\033[0;32m %10.2f \033[0m\n",
            ns2us(dur), (ns2us(dur) / nr_vpages), (ns2us(dur) / nr_hpages), (region_GBs / seconds));
    } else {
        printf("done. elapsed us: %12.2f    per-vpage us: %10.2f    per-hpage us: %10.2f    GB/s: %10.2f\n",
            ns2us(dur), (ns2us(dur) / nr_vpages), (ns2us(dur) / nr_hpages), (region_GBs / seconds));
    }
}

void print_size(ssize_t size) {
    char *c;

    c = malloc(sizeof(char) * 1024);
    bytes2str(size, c);
    printf("Total Region Size: %s\n", c);
    free(c);
}

void *allocate(ssize_t region_size, int node) {
    uint64_t  start, dur;
    void     *region;

    start  = getns();

    region = numa_alloc_onnode(region_size, node);

    dur = getns() - start;
    print_time_stats(dur, region_size, 0);

    return region;
}

/* void *allocate(ssize_t region_size, const unsigned long *nodemask, unsigned long max_nodemask) { */
/*     uint64_t  start, dur; */
/*     void     *region; */

/*     start  = getns(); */

/*     region = mmap(NULL, region_size, (PROT_READ | PROT_WRITE), */
/*                     (MAP_PRIVATE | MAP_ANONYMOUS), -1, 0); */
/*     if (region == MAP_FAILED) { */
/*         perror ("region1 mmap failed"); */
/*         exit(EXIT_FAILURE); */
/*     } */

/*     if (mbind(region, region_size, MPOL_BIND, nodemask, max_nodemask, 0) < 0) { */
/*         perror ("mbind failed"); */
/*         exit(EXIT_FAILURE); */
/*     } */

/*     dur = getns() - start; */
/*     print_time_stats(dur, region_size, 0); */

/*     return region; */
/* } */

void populate_region(void *region, ssize_t region_size, char val) {
    uint64_t start, dur;

    start = getns();
    memset(region, val, region_size);
    dur = getns() - start;

    print_time_stats(dur, region_size, 0);
}

uint64_t access_region(void *region, ssize_t region_size) {
    uint64_t cur, end;
    uint64_t start, dur;
    uint64_t val, ret = 0;

    cur = (uint64_t) region;
    end = ((uint64_t) region + region_size);

    start = getns();
    for (; cur < end; cur += sizeof(uint64_t)) {
        val  = *((uint64_t*)cur);
        ret += val;
    }
    dur = getns() - start;

    print_time_stats(dur, region_size, 0);

    return ret;
}
