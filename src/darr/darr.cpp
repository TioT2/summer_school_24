#include "darr.h"

typedef struct __DarrHeader {
  size_t elementSize;
  size_t capacity;
  size_t initialSize;
} DarrHeader;

void * __darrAlloc( size_t elementSize, size_t initialSize ) {
}

void darrFree( void *array ) {

}

void * darrResize( void *array, size_t newSize ) {

}
