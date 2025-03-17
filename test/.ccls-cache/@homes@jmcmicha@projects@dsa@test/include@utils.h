#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <numa.h>
#include <numaif.h>
#include <time.h>
#include <sys/mman.h>

#define DRAM_NODEMASK    (0x1ul)  //Node 0
// #define DRAM_NODEMASK    (0x20ul)  //Node 0
// #define PMEM_NODEMASK    (0x1ul)  //Node 0
// #define PMEM_NODEMASK    (0x2ul) //Node 1
// #define PMEM_NODEMASK    (0x10ul) //Node 4
#define PMEM_NODEMASK    (0x20ul) //Node 5

// #define MAX_NODEMASK (0x3ul) //Node1
#define MAX_NODEMASK (0xfful) //Node4

#define DRAM_NODE        0
// #define PMEM_NODE        0
// #define PMEM_NODE        1
// #define PMEM_NODE        2
// #define PMEM_NODE        4
#define PMEM_NODE        5

#define VMEM_PAGE_SIZE   4096       //4KB
// #define VMEM_PAGE_SIZE   1048576    //1MB
// #define VMEM_PAGE_SIZE   2097152    //2MB
// #define VMEM_PAGE_SIZE   1073741824 //1GB

#define VERBOSE

#define VMEM_H_PAGE_SIZE 4096 //4k
#define MAX_JOBS         16
#define MAX_BATCH_SIZE   128

uint64_t getns(void);
double   ns2us(uint64_t ns);
double   ns2ms(uint64_t ns);
void     bytes2str(size_t bytes, char *str);
void     print_time_stats(uint64_t dur, ssize_t region_size, int color);
void    *allocate(ssize_t region_size, int node);
void     populate_region(void *region, ssize_t region_size, char val);
uint64_t access_region(void *region, ssize_t region_size);

#endif
