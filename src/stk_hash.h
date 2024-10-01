/**
 * @brief hasher implementation function
 */

#ifndef STK_HASH_H_
#define STK_HASH_H_

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/// @brief SHA256 hash representation structure
typedef struct __StkHash {
    uint32_t hash[8]; ///< hash numbers
} StkHash;

/**
 * @brief hash calculation function
 * 
 * @param[in] data data to calculate hash of pointer (non-nullable)
 * @param[in] size data size (in bytes)
 * 
 * @return hash
 */
StkHash
stkHash( const void *data, const size_t size );

#endif // !defined(STK_HASH_H_)

// stk_hash.h file end
