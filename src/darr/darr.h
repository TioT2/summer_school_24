#ifndef DARR_H_
#define DARR_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void * _darrCreate( size_t elementSize, size_t initialSize );

#define darrCreate(type, initialSize) ((type *)_darrCreate(sizeof(type), (initialSize)))

void * darrReserve( void *array, size_t elementCount );

void * darrTruncCapacity( void *array );

void * darrToArray( void *array );

void darrDestroy( void *array );

#endif // !defined(DARR_H_)

// darr.h file end