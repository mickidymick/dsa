/*******************************************************************************
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "dml.h"

/* #define BUFFER_SIZE 1024 // 1 KB */
/* #define BUFFER_SIZE 4096    // 4 KB */
/* #define BUFFER_SIZE 2097152 // 2 MB */
/* #define BUFFER_SIZE 4194304 //4 MB */
/* #define BUFFER_SIZE 8388608 //8 MB */
/* #define BUFFER_SIZE 16777216 // 16 MB */
/* #define BUFFER_SIZE 1073741824 // 1 GB */

/*
* This example demonstrates how to create and run a mem_move operation
*/

uint64_t getns(void) {
    struct timespec ts;
    int             ret;

    ret = clock_gettime(CLOCK_MONOTONIC, &ts);
    assert(ret == 0);

    return (((uint64_t)ts.tv_sec) * 1000000000ULL) + ts.tv_nsec;
}

double ns2us(uint64_t ns) {
    return ((double)ns/1000.0);
}

void print_time_stats(uint64_t dur) {
    ssize_t nr_pages;
    double  seconds, region_GBs;
    long long buff = 1024;
/*     long long buff = 2097152; */
/*     long long buff = 1073741824; */

    region_GBs = (buff / (1024.0 * 1024.0 * 1024));
/*     region_GBs = (buff / (1024.0 * 1024.0 * 1024)); */
    seconds    = ((double)(dur) / (1000.0 * 1000.0 * 1000.0));

    printf("done. elapsed us: %12.2f   GB/s: %10.2f\n", ns2us(dur), (region_GBs / seconds));
}

int main(int argc, char **argv)
{
    long long buff = 1024;
/*     long long buff = 2097152; */
/*     long long buff = 1073741824; */

    uint64_t start, dur;

    dml_path_t execution_path = DML_PATH_HW;

    printf("Starting memory move job example.\n");
/*     printf("Moving region of size %lldB.\n",buff); */

    dml_job_t *dml_job_ptr = NULL;
    uint32_t size = 0u;

    uint8_t source      [buff];
    uint8_t destination [buff];

    memset((void *)source, 0, sizeof(uint8_t) * buff);
    memset((void *)destination, 0, sizeof(uint8_t) * buff);

    for(int i = 0; i < buff; i++){
        source[i] = i % 256;
    }

    for(int i = 0; i < buff; i++){
        destination[i] = 0;
    }

/*     start = getns(); */

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
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY;
    dml_job_ptr->source_first_ptr       = source;
    dml_job_ptr->destination_first_ptr  = destination;
    dml_job_ptr->source_length          = buff;
    dml_job_ptr->destination_length     = buff;

    start = getns();

    status = dml_execute_job(dml_job_ptr, DML_WAIT_MODE_BUSY_POLL);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job execution.\n", status);
        dml_finalize_job(dml_job_ptr);
        free(dml_job_ptr);
        return 1;
    }

    dur = getns() - start;
    print_time_stats(dur);

    for(int i = 0; i < buff; i++){
        if(destination[i] != source[i]){
            printf("Error: Operation result is incorrect.\n");
            dml_finalize_job(dml_job_ptr);
            free(dml_job_ptr);
            return 1;
        }
    }

    status = dml_finalize_job(dml_job_ptr);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        free(dml_job_ptr);
        return 1;
    }
    free(dml_job_ptr);

    printf("Job Completed Successfully.\n");
    return 0;
}
