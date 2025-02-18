#ifndef DSA_H
#define DSA_H

#define _GNU_SOURCE
#include <pthread.h>
#include "utils.h"
#include "dml.h"

//Helper Functions
int        dml_check(dml_job_t *job_ptr);

//Basic DSA Calls
int        _dsa_sync_copy(void *dest, void *src, uint64_t buffer_size);
dml_job_t *_dsa_async_copy_start(void *dest, void *src, uint64_t buffer_size, int node);
int        _dsa_async_copy_end(dml_job_t *dml_job_ptr);
dml_job_t *_dsa_async_batch_copy_start(void *dest, void *src, uint64_t buffer_size, dml_path_t execution_path, uint32_t job_size, int batch_size, int node);
int        _dsa_async_batch_copy_end(dml_job_t *dml_job_ptr);
void      *_dsa_threaded_async_copy(void *arguments);

//MAIN DSA Wrapper Functions
int        cpu_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size);
int        dsa_sync_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size);
int        dsa_async_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size);
int        dsa_batched_async_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size);
int        dsa_threaded_async_copy(void *dest, void *src, uint64_t region_size, uint64_t transfer_size);

struct arg_struct {
    int   node;
    void *src;
    void *dest;
    uint64_t buffer_size;
};

/*
typedef enum
{
    // General Statuses
    DML_STATUS_OK                               = 0u,    Operation completed successfully.
    DML_STATUS_FALSE_PREDICATE_OK               = 1u,    Success with false predicate.
    DML_STATUS_BEING_PROCESSED                  = 2u,    Operation is still being processed.
    DML_STATUS_INTERNAL_ERROR                   = 3u,    Unexpected internal error condition.
    DML_STATUS_NULL_POINTER_ERROR               = 4u,    Null pointer error.
    DML_STATUS_LIMITS_ERROR                     = 5u,    One of the job parameters exceeds configured DML limits.
    DML_STATUS_PATH_ERROR                       = 6u,    Invalid dmlPath parameter.
    DML_STATUS_HINT_ERROR                       = 7u,    Invalid hint parameter.
    DML_STATUS_CRC_ALIGN_ERROR                  = 8u,    crc_checksum_ptr must be 4-byte aligned
    DML_STATUS_BATCH_ERROR                      = 9u,    One or more operation in the batch completed with status > 0.
    DML_STATUS_DELTA_ASCENDT_ERROR              = 10u,   Offsets in the delta record were not in increasing order.
    DML_STATUS_DELTA_OFFSET_ERROR               = 11u,   The input length is greater than max available offset
    DML_STATUS_DIF_CHECK_ERROR                  = 12u,   DIF check failed.
    DML_STATUS_JOB_OPERATION_ERROR              = 13u,   Invalid op field in dml_job_t.
    DML_STATUS_JOB_FLAGS_ERROR                  = 14u,   Invalid flags field in dml_job_t.
    DML_STATUS_JOB_LENGTH_ERROR                 = 15u,   Invalid length field in dml_job_t.
    DML_STATUS_BATCH_LIMITS_ERROR               = 16u,   Invalid number of Jobs for batch operation (LT than 2 or GT than max batch size)
    DML_STATUS_DELTA_RECORD_SIZE_ERROR          = 17u,   Delta Record Size is out of range or delta record size is not multiple of 10.
    DML_STATUS_OVERLAPPING_BUFFER_ERROR         = 18u,   Overlapping buffers.
    DML_STATUS_DUALCAST_ALIGN_ERROR             = 19u,   Bit 11:0 of pDst1 and pDst2 differ in Memory Copy with Dualcast operation.
    DML_STATUS_DELTA_ALIGN_ERROR                = 20u,   Misaligned address for Delta Record: pSrc1, pSrc2, pDst1 or length is not 8 - byte aligned.
    DML_STATUS_DELTA_INPUT_SIZE_ERROR           = 21u,   Input size is not multiple of 8 for delta operations.
    DML_STATUS_MEMORY_OVERFLOW_ERROR            = 22u,   Buffer detected an overflow.
    DML_STATUS_JOB_CORRUPTED                    = 23u,   @ref dml_job_t structure is corrupted
    DML_STATUS_WORK_QUEUE_OVERFLOW_ERROR        = 24u,   WQ overflow.
    DML_STATUS_PAGE_FAULT_ERROR                 = 25u,   Page Fault occurred during processing on hardware
    DML_STATUS_TC_A_NOT_AVAILABLE               = 26u,   WQs configured to work with TC-A are not visible
    DML_STATUS_TC_B_NOT_AVAILABLE               = 27u,   WQs configured to work with TC-B are not visible
    DML_STATUS_BATCH_TASK_INDEX_OVERFLOW        = 28u,   Batch task index is bigger than size of the batch
    DML_STATUS_BATCH_SIZE_ERROR                 = 29u,   The desired batch size is either bigger or smaller than the possible one. Refer to DML_MIN_BATCH_SIZE for minimal supported batch size.
    DML_STATUS_DRAIN_PAGE_FAULT_ERROR           = 30u,   A page fault occurred while translating a Readback Address in a Drain descriptor
    DML_STATUS_UNKNOWN_CACHE_SIZE_ERROR         = 31u,   Max cache size can't be calculated
    DML_STATUS_DIF_STRIP_ADJACENT_ERROR         = 32u,   SRC Address for DIF Strip operation should be greater than (DST Address + SRC Size)
    DML_STATUS_INTL_INVALID_PAGE_REQUEST        = 33u,   Internal Status Code
    DML_STATUS_INTL_INVALID_RESERVED_FIELD      = 34u,   Internal Status Code
    DML_STATUS_INTL_MISALIGNED_DESC_LA          = 35u,   Internal Status Code
    DML_STATUS_INTL_INVALID_COMPLETION_HANDLE   = 36u,   Internal Status Code
    DML_STATUS_INTL_PAGE_FAULT_ON_TRANSLATION   = 37u,   Internal Status Code
    DML_STATUS_INTL_MISALIGNED_CR_ADDRESS       = 38u,   Internal Status Code
    DML_STATUS_INTL_MISALIGNED_ADDRESS          = 39u,   Internal Status Code
    DML_STATUS_INTL_PRIVILEGE_ERROR             = 40u,   Internal Status Code
    DML_STATUS_INTL_TRAFFIC_CLASS_ERROR         = 41u,   Internal Status Code
    DML_STATUS_INTL_READBACK_ADDRESS_PAGE_ERROR = 42u,   Internal Status Code
    DML_STATUS_INTL_HARDWARE_READBACK_TIMEOUT   = 43u,   Internal Status Code
    DML_STATUS_INTL_HARDWARE_TIMEOUT            = 44u,   Internal Status Code
    DML_STATUS_INTL_ADDRESS_TRANSLATION_ERROR   = 45u,   Internal Status Code
    DML_STATUS_NOT_SUPPORTED_BY_WQ              = 46u,   Work queue not configured to support operation

    // Initialization Errors
    DML_STATUS_LIBACCEL_NOT_FOUND               = 100u (DML_BASE_DRIVER_ERROR + 0u),   Unable to initialize job because hardware driver was not found
    DML_STATUS_LIBACCEL_ERROR                   = 101u (DML_BASE_DRIVER_ERROR + 1u),   Unable to initialize job because hardware driver API is incompatible
    DML_STATUS_WORK_QUEUES_NOT_AVAILABLE        = 102u (DML_BASE_DRIVER_ERROR + 2u),   Enabled work queues are not found
    DML_STATUS_INIT_HW_NOT_SUPPORTED            = 103u (DML_BASE_DRIVER_ERROR + 3u),   Error occured on hw initialization due to failure to write() to wq
} dml_status_t;
*/

#endif
