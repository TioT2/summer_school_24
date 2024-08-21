#ifndef DARR_H_
#define DARR_H_

#include <stdlib.h>

void * __darrAlloc( size_t elementSize, size_t initialSize );

#define darrAlloc(type, initialSize) ((type *)__darrAlloc(sizeof(type), initialSize))

void darrFree( void *array );

void * darrResize( void *array, size_t newSize );

#endif // !defined(DARR_H_)