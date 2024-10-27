/**
 * @brief memory availability checker declaration module
 */

#ifndef STK_MEMORY_H_
#define STK_MEMORY_H_

#include <stdint.h>
#include <stdbool.h>

/// memory access info bits
typedef enum __StkMemoryAccessBits {
    /// memory reading allowed
    STK_MEMORY_ACCESS_READ  = 1,

    /// memory writing allowed
    STK_MEMORY_ACCESS_WRITE = 2,
} StkMemoryAccessBits;

typedef uint32_t StkMemoryAccess;

/**
 * @brief memory access info getting function
 * 
 * @param memory     memory to check access parameters of
 * @param regionSize memory region
 * @param dst        memory access flag destination pointer (non-null)
 * 
 * @return true if written, false if undefined.
 */
bool
stkGetMemoryAccessParameters( const void *memory, size_t regionSize, StkMemoryAccess *dst );

#endif // !defined(STK_MEMORY_H_)

// stk_memory.h file end
