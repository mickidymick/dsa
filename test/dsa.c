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
int dsa_copy(void *dest, void *src, uint64_t buffer_size) {
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
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY;
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
dml_job_t *dsa_async_copy_start(void *dest, void *src, uint64_t buffer_size, int node) {
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
    dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY;
/*     dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT; */
    dml_job_ptr->source_first_ptr       = (uint8_t *)src;
    dml_job_ptr->destination_first_ptr  = (uint8_t *)dest;
    dml_job_ptr->source_length          = buffer_size;
    dml_job_ptr->destination_length     = buffer_size;
    dml_job_ptr->numa_id                = node;

    status = dml_submit_job(dml_job_ptr);

    if (status != DML_STATUS_OK) {
        printf("An error (%u) occurred during submitting job.\n", status);
        free(dml_job_ptr);
        return NULL;
    }

    return dml_job_ptr;
}

//Asyncronus DSA Copy Job End
int dsa_async_copy_end(dml_job_t *dml_job_ptr) {
    dml_status_t status = dml_finalize_job(dml_job_ptr);

    free(dml_job_ptr);

    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job finalization.\n", status);
        return 1;
    }

    return 0;
}

//Threaded Syncronus DSA Copy
void *dsa_threaded_copy(void *arguments) {
    struct arg_struct *args        = arguments;
    void              *src         = args->src;
    void              *dest        = args->dest;
    uint64_t          buffer_size  = args->buffer_size;

    dml_path_t  execution_path = DML_PATH_HW; //hardware path
    dml_job_t  *dml_job_ptr    = NULL;
    uint32_t    size           = 0u;

    for (uint64_t i = 0; i < buffer_size; i += VMEM_PAGE_SIZE) {
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
        dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY;
/*         dml_job_ptr->flags                  = DML_FLAG_COPY_ONLY | DML_FLAG_BLOCK_ON_FAULT; */
        dml_job_ptr->source_first_ptr       = (uint8_t *)(src + i);
        dml_job_ptr->destination_first_ptr  = (uint8_t *)(dest + i);
        dml_job_ptr->source_length          = VMEM_PAGE_SIZE;
        dml_job_ptr->destination_length     = VMEM_PAGE_SIZE;
        dml_job_ptr->numa_id                = args->node;

        status = dml_execute_job(dml_job_ptr, DML_WAIT_MODE_BUSY_POLL);
        if (DML_STATUS_OK != status) {
            printf("An error (%u) occurred during job execution.\n", status);
            dml_finalize_job(dml_job_ptr);
            free(dml_job_ptr);
            return NULL;
        }

        status = dml_finalize_job(dml_job_ptr);

        free(dml_job_ptr);

        if (DML_STATUS_OK != status) {
            printf("An error (%u) occurred during job finalization.\n", status);
            return NULL;
        }
    }
}

//Threaded Asyncronus DSA Copy
void *dsa_threaded_a_copy(void *arguments) {
    struct arg_struct *args        = arguments;
    void              *src         = args->src;
    void              *dest        = args->dest;
    uint64_t          buffer_size  = args->buffer_size;
    int               node         = args->node;

    dml_path_t  execution_path = DML_PATH_HW; //hardware path
    dml_job_t  *dml_job_ptr    = NULL;
    uint32_t    size           = 0u;

    uint64_t num_jobs_running = 0;
    uint64_t dml_arr_size = (uint64_t)buffer_size / (uint64_t)VMEM_PAGE_SIZE;
    dml_job_t **job_arr = malloc(sizeof(dml_job_t *) * dml_arr_size);
    for (uint64_t i = 0; i < dml_arr_size; i++) {
        job_arr[i] = NULL;
    }

    uint64_t k = 0;
    for( uint64_t i = 0; i < buffer_size; i += VMEM_PAGE_SIZE) {
        job_arr[k] = dsa_async_copy_start(dest + i, src + i, VMEM_PAGE_SIZE, node);
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
}

int dsa_batch_copy(void *dest, void *src, uint64_t buffer_size) {
    dml_path_t  execution_path      = DML_PATH_HW; //hardware path
    dml_job_t  *dml_job_ptr         = NULL;
    uint32_t    size                = 0u;
    uint32_t    batch_buffer_length = 0u;

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

    status = dml_get_batch_size(dml_job_ptr, 1u, &batch_buffer_length);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during getting batch size.\n", status);
        return 1;
    }

    uint8_t * batch_buffer_ptr = (uint8_t *) malloc(batch_buffer_length);

    dml_job_ptr->operation              = DML_OP_BATCH;
    dml_job_ptr->destination_first_ptr  = batch_buffer_ptr;
    dml_job_ptr->destination_length     = batch_buffer_length;

    status = dml_batch_set_mem_move_by_index(dml_job_ptr, 0, src, dest, buffer_size, DML_FLAG_COPY_ONLY);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during job batching.\n", status);
        dml_finalize_job(dml_job_ptr);
        free(dml_job_ptr);
        return 1;
    }

    status = dml_batch_set_nop_by_index(dml_job_ptr, 4, DML_FLAG_FENCE);
    if (DML_STATUS_OK != status) {
        printf("An error (%u) occurred during setting fence of batch operation.\n", status);
        free(dml_job_ptr);
        return 1;
    }

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




//   ---Main DSA Wrapper Functions---
//Non-Threaded Asyncronus DSA Copy
int dsa_loop_async_copy(void *dest, void *src, uint64_t buffer_size) {
    int        dml_arr_max_size = MAX_JOBS;
    int        dml_arr_size     = 0;
    int        num_pages        = buffer_size / VMEM_PAGE_SIZE;
    dml_job_t *tmp_dml_job_ptr;
    dml_job_t *job_arr[dml_arr_max_size];

    for (int i = 0; i < dml_arr_max_size; i++) {
        job_arr[i] = NULL;
    }

    //loop over region page by page
    for (uint64_t i = 0; i < num_pages; i++) {
/*         printf("page:%lld total:%d\n", i, num_pages); */

        //if we can submit a new job
        if (dml_arr_size < dml_arr_max_size) {
do_copy:
/*             printf("start copy\n"); */
            tmp_dml_job_ptr = dsa_async_copy_start(dest + (i * VMEM_PAGE_SIZE), src + (i * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE, 0);
/*             tmp_dml_job_ptr = dsa_async_copy_start(dest + (i * VMEM_PAGE_SIZE), src + (i * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE, i % 4); */
/*             printf("job_ptr:%p\n", tmp_dml_job_ptr); */
            if (tmp_dml_job_ptr == NULL) {
                printf("ERROR: no job_ptr returned from dsa_async_copy_start!\n");
                for (int k = 0; k < dml_arr_max_size; k++) {
                    if (job_arr[k] != NULL) {
                        free(job_arr[k]);
                    }
                }
                return 1;
            }

            //add job to arr
            for (int j = 0; j < dml_arr_max_size; j++) {
                if (job_arr[j] == NULL) {
                    job_arr[j]    = tmp_dml_job_ptr;
                    dml_arr_size += 1;
/*                     printf("added job\n"); */
                    break;
                }
            }

        } else {
            while (dml_arr_size > 0) {
/*                 printf("inside\n"); */
/*                 printf("arr_size:%d\n", dml_arr_size); */
/*                 printf("looking for completed job\n"); */
                for (int j = 0; j < dml_arr_max_size; j++) {
                    int status = dml_check(job_arr[j]);
                    //job completed
                    if (status == 1) {
/*                         printf("job completed\n"); */
                        if (dsa_async_copy_end(job_arr[j]) == 1) {
                            for (int k = 0; k < dml_arr_max_size; k++) {
                                if (job_arr[k] != NULL) {
                                    free(job_arr[k]);
                                }
                            }
                            return 1;
                        }
                        job_arr[j] = NULL;
                        dml_arr_size -= 1;
                        goto do_copy;
                    //job errored
                    } else if (status != 0){
                        printf("An error (%u) occurred during job finalization.\n", dml_check_job(job_arr[i]));
                        for (int k = 0; k < dml_arr_max_size; k++) {
                            if (job_arr[k] != NULL) {
                                free(job_arr[k]);
                            }
                        }
                        return 1;
                    }
                }
/*                 printf("waiting\n"); */
            }
        }
    }

/*     printf("outside\n"); */
    while (dml_arr_size > 0) {
/*         printf("arr_size:%d\n", dml_arr_size); */
/*         printf("looking for completed job\n"); */
        for (int j = 0; j < dml_arr_max_size; j++) {
/*             printf("job_arr[%d]: %p\n", i, job_arr[i]); */
            if (job_arr[j] != NULL) {
                int status = dml_check(job_arr[j]);
                //job completed
                if (status == 1) {
/*                     printf("job completed\n"); */
                    dsa_async_copy_end(job_arr[j]);
                    job_arr[j] = NULL;
                    dml_arr_size -= 1;
                //job errored
                } else if (status != 0){
                    printf("An error (%u) occurred during job finalization.\n", dml_check_job(job_arr[j]));
                    for (int k = 0; k < dml_arr_max_size; k++) {
                        if (job_arr[k] != NULL) {
                            free(job_arr[k]);
                        }
                    }
                    return 1;
                }
            }
        }
/*         printf("waiting\n"); */
    }

    return 0;
}

//Threaded Syncronus DSA Copy
int dsa_threaded_sync_copy(void *dest, void *src, uint64_t buffer_size) {
    int                threads = MAX_JOBS;
    pthread_t          th_arr[threads];
    struct arg_struct *a_arr[threads];

    uint64_t size = buffer_size / threads;

    for (int i = 0; i < threads; i++) {
        a_arr[i]              = malloc(sizeof(struct arg_struct) * 1);
        a_arr[i]->node        = i % 4;
        a_arr[i]->src         = src + (i * size);
        a_arr[i]->dest        = dest + (i * size);
        a_arr[i]->buffer_size = size;

        if (pthread_create(&th_arr[i], NULL, dsa_threaded_copy, a_arr[i]) != 0) {
            printf("Pthread Create Failed\n");
            return 1;
        }
    }
    for (int i = 0; i < threads; i++) {
        pthread_join(th_arr[i], NULL);
    }

    return 0;
}

//Threaded Asyncronus DSA Copy
int dsa_threaded_async_copy(void *dest, void *src, uint64_t buffer_size) {
    int                threads = MAX_JOBS;
    pthread_t          th_arr[threads];
    struct arg_struct *a_arr[threads];

    uint64_t size = buffer_size / threads;

    for (int i = 0; i < threads; i++) {
        a_arr[i]              = malloc(sizeof(struct arg_struct) * 1);
        a_arr[i]->src         = src + (i * size);
        a_arr[i]->dest        = dest + (i * size);
        a_arr[i]->buffer_size = size;
        a_arr[i]->node        = i % 1;

        pthread_create(&th_arr[i], NULL, dsa_threaded_a_copy, a_arr[i]);
    }
    for (int i = 0; i < threads; i++) {
        pthread_join(th_arr[i], NULL);
    }

    return 1;
}

//Batched Syncronus DSA Copy
int dsa_batched_sync_copy(void *dest, void *src, uint64_t buffer_size) {
    uint64_t batch_count = (buffer_size / VMEM_PAGE_SIZE) / MAX_BATCH_SIZE;

/*     printf("\n"); */
    for (uint64_t i = 0; i < batch_count; i++) {
        if (dsa_batch_copy(dest + (i * VMEM_PAGE_SIZE * MAX_BATCH_SIZE), src + (i * VMEM_PAGE_SIZE * MAX_BATCH_SIZE), VMEM_PAGE_SIZE) != 0) {
/*             printf("iter:%2lu dest:%p src:%p buffer_size:%d batch_count:%d\n", i, dest + (i * VMEM_PAGE_SIZE * MAX_BATCH_SIZE), src + (i * VMEM_PAGE_SIZE * MAX_BATCH_SIZE), VMEM_PAGE_SIZE, MAX_BATCH_SIZE); */
        }
    }

    return 0;
}
