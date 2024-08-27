#ifndef DARR_H_
#define DARR_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DARR_API __cdecl

/**
 * @brief dynamic array constructor
 * 
 * @param elementSize array element size
 * @param initialSize initial array size
 * 
 * @return created dynamic array
 */
void * DARR_API
darrCreate( size_t elementSize, size_t initialSize );

/**
 * @brief element to array pushing function (extends array by 1 element)
 * 
 * @param array      array to push value to
 * @param elementPtr element data
 * 
 * @return reallocated array
 */
void * DARR_API
darrPush( void *array, const void *elementPtr );

/**
 * @brief array values reserve function (extends array by elementCount elements)
 * 
 * @param array        array to reserve values in
 * @param elementCount count of elements to reserve
 * 
 * @return reallocated array
 */
void * DARR_API
darrReserve( void *array, size_t elementCount );

/**
 * @brief array capacity to minimal required capacity truncation function
 * 
 * @param array array to truncate capacity of
 * 
 * @return reallocated array
 */
void * DARR_API
darrTruncCapacity( void *array );

/**
 * @brief dynamic to ordinary array (ok to free by 'free(arr)' call) transform function
 * 
 * @param array dynamic array to transform
 * 
 * @return ordinary array with same data
 */
void * DARR_API
darrToArray( void *array );

/**
 * @brief array destructor
 * 
 * @param array array to destroy
 */
void DARR_API
darrDestroy( void *array );

#endif // !defined(DARR_H_)

// darr.h file end
