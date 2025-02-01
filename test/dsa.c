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
#include "dml.h"

#define PAGE_SIZE  4096
#define TIME_INIT  clock_t start; clock_t end; double cpu_time_used;
#define TIME_START start = clock();
#define TIME_STOP  end = clock(); cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; printf("Time: %f seconds\n", cpu_time_used);

typedef struct page {
    char data[PAGE_SIZE];
} Page;

void call_move_pages(Page *mem, long long num_pages, int new_device);
int dsa_copy(Page *src, Page *dest, long long buffer_size, long long arr_size);
int dsa_mem(Page *src, Page *dest, long long buffer_size, long long arr_size);

int main(int argc, char* argv[]) {
    Page *mem_u;
    Page *mem_l;
    int upper_dev         = 0;
    int lower_dev         = 1;
    int upper_size_base_2 = 22;
    long long upper_size  = pow(2, upper_size_base_2);
    long long arr_size    = upper_size / PAGE_SIZE;

    TIME_INIT

    printf("Allocating Upper Tier\n");
    mem_u = (Page *)numa_alloc_onnode(upper_size, 0);
    if (mem_u == NULL) {
/*     if (mem_u == nullptr) { */
        printf("Memory Allocation Failed!\n");
        return 1;
    }
    printf("Fault in Pages\n");
    memset(mem_u, 1, upper_size);

    printf("Allocating Lower Tier\n");
    mem_l = (Page *)numa_alloc_onnode(upper_size, 1);
    if (mem_l == NULL) {
/*     if (mem_l == nullptr) { */
        printf("Memory Allocation Failed!\n");
        return 1;
    }
    printf("Fault in Pages\n\n");
    memset(mem_l, 2, upper_size);

    printf("Calling DSA Copy\n");
    TIME_START
    dsa_copy(mem_u, mem_l, upper_size, arr_size);
    TIME_STOP

    numa_free(mem_u, upper_size);
    numa_free(mem_l, upper_size);

    exit(0);
}

int dsa_copy(Page *src, Page *dest, long long buffer_size, long long arr_size) {
/*     dml_path_t execution_path = DML_PATH_SW; //software path */
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

    uint32_t crc = 1;

    dml_job_ptr->operation              = DML_OP_COPY_CRC;
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;
    dml_job_ptr->crc_checksum_ptr       = &crc;
    dml_job_ptr->flags                  = DML_FLAG_CRC_READ_SEED; // read value from crc_checksum_ptr as crc seed

    status = dml_execute_job(dml_job_ptr, DML_WAIT_MODE_BUSY_POLL);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job execution.\n", status);
        dml_finalize_job(dml_job_ptr);
        free(dml_job_ptr);
        return 1;
    }

/*     for(int i = 0; i < arr_size; i++){ */
/*         if(dest->data[i] != src->data[i]){ */
/*             printf("Error: Operation result is incorrect.\n"); */
/*             dml_finalize_job(dml_job_ptr); */
/*             free(dml_job_ptr); */
/*             return 1; */
/*         } */
/*     } */

/*     printf("Calculated CRC is: 0x%08x\n", crc); */

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
