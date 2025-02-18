#include "dsa.h"

//   ---Helper Functions---
//Checks Job Status
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



//   ---Basic DSA Calls---
//Syncronous DSA Copy
int _dsa_sync_copy(void *dest, void *src, uint64_t buffer_size) {
    uint64_t    start;
    uint64_t    dur;
    dml_path_t  execution_path = DML_PATH_HW; //hardware path
    dml_job_t  *dml_job_ptr    = NULL;
    uint32_t    size           = 0u;

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
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_PREFETCH_CACHE;
/*     dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT; */
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

    free(dml_job_ptr);

    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        return 1;
    }

    return 0;
}

//Asyncronus DSA Copy Job Start
dml_job_t *_dsa_async_copy_start(void *dest, void *src, uint64_t buffer_size, int node) {
    dml_path_t  execution_path = DML_PATH_HW; //hardware path
    dml_job_t  *dml_job_ptr    = NULL;
    uint32_t    size           = 0u;

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
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_PREFETCH_CACHE;
/*     dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT; */
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;
    dml_job_ptr->destination_length     = buffer_size;
/*     dml_job_ptr->numa_id                = node; */

    status = dml_submit_job(dml_job_ptr);
    while(status == DML_STATUS_WORK_QUEUES_NOT_AVAILABLE) {
        status = dml_submit_job(dml_job_ptr);
    }

    if (status != DML_STATUS_OK) {
        printf("An error (%u) occurred during submitting job.\n", status);
        free(dml_job_ptr);
        return NULL;
    }

    return dml_job_ptr;
}

//Asyncronus DSA Copy Job End
int _dsa_async_copy_end(dml_job_t *dml_job_ptr) {
    dml_status_t status = dml_finalize_job(dml_job_ptr);

    free(dml_job_ptr);

    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        return 1;
    }

    return 0;
}

//Asyncronus Batched DSA Copy Start
dml_job_t *_dsa_async_batch_copy_start(void *dest, void *src, uint64_t buffer_size, dml_path_t execution_path, uint32_t job_size, int batch_size, int node) {
    dml_status_t  status;
    dml_job_t    *dml_job_ptr         = NULL;
    uint32_t      batch_buffer_length = 0u;

    dml_job_ptr = (dml_job_t *)malloc(job_size);

    status = dml_init_job(execution_path, dml_job_ptr);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job initialization.\n", status);
        free(dml_job_ptr);
        return NULL;
    }

/*     printf("batch_size:%d\n", batch_size); */
    status = dml_get_batch_size(dml_job_ptr, batch_size, &batch_buffer_length);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during getting batch size.\n", status);
        return NULL;
    }

    uint8_t * batch_buffer_ptr = (uint8_t *) malloc(batch_buffer_length);

    dml_job_ptr->operation              = DML_OP_BATCH;
    dml_job_ptr->destination_first_ptr  = batch_buffer_ptr;
    dml_job_ptr->destination_length     = batch_buffer_length;

    for (int i = 0; i < batch_size; i++) {
        status = dml_batch_set_mem_move_by_index(dml_job_ptr, i, src + (i * buffer_size), dest + (i * buffer_size), buffer_size, DML_FLAG_COPY_ONLY);
        if (DML_STATUS_OK != status) {
            printf("An error (%u) occurred during job batching.\n", status);
            dml_finalize_job(dml_job_ptr);
            free(dml_job_ptr);
            return NULL;
        }
    }

    status = dml_submit_job(dml_job_ptr);
    while(status == DML_STATUS_WORK_QUEUES_NOT_AVAILABLE) {
        status = dml_submit_job(dml_job_ptr);
    }
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job execution.\n", status);
        dml_finalize_job(dml_job_ptr);
        free(dml_job_ptr);
        return NULL;
    }

    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        return NULL;
    }

    return dml_job_ptr;
}

//Asyncronus Batched DSA Copy End
int _dsa_async_batch_copy_end(dml_job_t *dml_job_ptr) {
    dml_status_t status = dml_finalize_job(dml_job_ptr);

    free(dml_job_ptr->destination_first_ptr);
    free(dml_job_ptr);

    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        return 1;
    }

    return 0;
}

//Threaded Asyncronus DSA Copy
void *_dsa_threaded_async_copy(void *arguments) {
    struct arg_struct *args        = arguments;
    void              *src         = args->src;
    void              *dest        = args->dest;
    uint64_t          buffer_size  = args->buffer_size;
    int               node         = args->node;

    dml_path_t  execution_path   = DML_PATH_HW; //hardware path
    dml_job_t  *dml_job_ptr      = NULL;
    uint32_t    size             = 0u;
    uint64_t    num_jobs_running = 0;
    uint64_t dml_arr_size        = (uint64_t)buffer_size / (uint64_t)VMEM_PAGE_SIZE;
    dml_job_t **job_arr          = malloc(sizeof(dml_job_t *) * dml_arr_size);

    for (uint64_t i = 0; i < dml_arr_size; i++) {
        job_arr[i] = NULL;
    }

    uint64_t k = 0;
    for( uint64_t i = 0; i < buffer_size; i += VMEM_PAGE_SIZE) {
        job_arr[k] = _dsa_async_copy_start(dest + i, src + i, VMEM_PAGE_SIZE, node);
        num_jobs_running++;
        k++;
    }

    while (num_jobs_running > 0) {
        for (uint64_t i = 0; i < dml_arr_size; i++) {
            if (job_arr[i] != NULL && dml_check(job_arr[i]) == 1) {
                _dsa_async_copy_end(job_arr[i]);
                job_arr[i] = NULL;
                num_jobs_running--;
            }
        }
    }

    free(job_arr);
}

//   ---Main DSA Wrapper Functions---
//CPU Memory Copy
int cpu_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size) {
    for( uint64_t i = 0; i < region_size; i += VMEM_PAGE_SIZE) {
        memcpy(dest + i, src + i, transfer_size);
    }

    return 0;
}

//Syncronus DSA Copy
int dsa_sync_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size) {
    for( uint64_t i = 0; i < region_size; i += VMEM_PAGE_SIZE) {
        if(_dsa_sync_copy(dest + i, src + i, transfer_size)) {
            return 1;
        }
    }

    return 0;
}

//Asyncronus DSA Copy
int dsa_async_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size) {
    uint64_t    k                = 0;
    uint64_t    num_jobs_running = 0;
    uint64_t    dml_arr_size     = region_size / transfer_size;
    dml_job_t **job_arr          = malloc(sizeof(dml_job_t *) * dml_arr_size);

    for (uint64_t i = 0; i < dml_arr_size; i++) {
        job_arr[i] = NULL;
    }

    for( uint64_t i = 0; i < region_size; i += VMEM_PAGE_SIZE) {
        job_arr[k] = _dsa_async_copy_start(dest + i, src + i, transfer_size, i % 4);
        if (job_arr[k] == NULL) {
            return 1;
        }
        num_jobs_running++;
        k++;
    }

    while (num_jobs_running > 0) {
        for (uint64_t i = 0; i < dml_arr_size; i++) {
            if (job_arr[i] != NULL && dml_check(job_arr[i]) == 1) {
                if (_dsa_async_copy_end(job_arr[i]) == 1) {
                    return 1;
                }
                job_arr[i] = NULL;
                num_jobs_running--;
            }
        }
    }

    free(job_arr);
    return 0;
}

//Batched Asyncronus DSA Copy
int dsa_batched_async_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size) {
    dml_path_t execution_path   = DML_PATH_HW; //hardware path
    uint32_t   job_size         = 0u;
    uint64_t   num_jobs_running = 0;
    uint64_t   num_batches      = region_size / transfer_size / MAX_BATCH_SIZE;
    uint64_t   remainder        = (region_size / transfer_size) % MAX_BATCH_SIZE;
    uint64_t   size             = transfer_size * MAX_BATCH_SIZE;

    if (remainder > 0) {
        num_batches++;
    }

    uint64_t dml_arr_size = num_batches;
    dml_job_t **job_arr   = malloc(sizeof(dml_job_t *) * dml_arr_size);

    for (uint64_t i = 0; i < dml_arr_size; i++) {
        job_arr[i] = NULL;
    }

    dml_status_t status = dml_get_job_size(execution_path, &job_size);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during getting job size.\n", status);
        return 0;
    }

    uint64_t k = 0;
    for( uint64_t i = 0; i < region_size; i += size) {
        if (remainder > 0 && k == num_batches - 1) {
            job_arr[k] = _dsa_async_batch_copy_start(dest + i, src + i, VMEM_PAGE_SIZE, execution_path, job_size, remainder, i % 4);
        } else {
            job_arr[k] = _dsa_async_batch_copy_start(dest + i, src + i, VMEM_PAGE_SIZE, execution_path, job_size, MAX_BATCH_SIZE, i % 4);
        }
        if (job_arr[k] == NULL) {
            return 1;
        }
        num_jobs_running++;
        k++;
    }

    while (num_jobs_running > 0) {
        for (uint64_t i = 0; i < dml_arr_size; i++) {
            if (job_arr[i] != NULL && dml_check(job_arr[i]) == 1) {
                if (_dsa_async_batch_copy_end(job_arr[i]) == 1) {
                    return 1;
                }
                job_arr[i] = NULL;
                num_jobs_running--;
            }
        }
    }

    free(job_arr);
    return 0;
}

//Threaded Asyncronus DSA Copy
int dsa_threaded_async_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size) {
    int                threads = MAX_JOBS;
    int                num_threads;
    pthread_t          th_arr[threads];
    struct arg_struct *a_arr[threads];
    uint64_t           size;
    uint64_t           remainder;
    cpu_set_t          cpuset;
    pthread_attr_t     attr;

    //calculate number of threads up to MAX_JOBS
    num_threads = region_size / transfer_size;
    if (num_threads < MAX_JOBS) {
        threads = num_threads;
    }

    size       = region_size / threads;
    remainder  = (region_size) % threads;
    uint64_t s = region_size / transfer_size / threads;

    for (int i = 0; i < threads; i++) {
        if (i == threads - 1) {
            size += remainder;
        }

        a_arr[i]              = malloc(sizeof(struct arg_struct) * 1);
        a_arr[i]->src         = src + (i * size);
        a_arr[i]->dest        = dest + (i * size);
        a_arr[i]->buffer_size = size;
        a_arr[i]->node        = i % 4;

        pthread_attr_init(&attr);
        CPU_ZERO(&cpuset);
        CPU_SET(i, &cpuset);
        pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
        pthread_create(&th_arr[i], &attr, _dsa_threaded_async_copy, a_arr[i]);
    }
    for (int i = 0; i < threads; i++) {
        pthread_join(th_arr[i], NULL);
    }

    return 1;
}
