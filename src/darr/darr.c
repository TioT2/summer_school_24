#include "darr.h"

typedef struct __DarrHeader {
  size_t elementSize;
  size_t capacity;
  size_t size;
} DarrHeader;

void * _darrCreate( size_t elementSize, size_t initialSize ) {
  assert(elementSize != 0);

  size_t capacity = 1;

  while (capacity < initialSize) {
    initialSize *= 2;
  }

  DarrHeader *header = (DarrHeader *)malloc(sizeof(DarrHeader) + elementSize * capacity);

  if (header == NULL)
    return NULL;

  header->elementSize = elementSize;
  header->capacity = capacity;
  header->size = initialSize;

  return header + 1;
}


void * darrReserve( void *array, size_t elementCount ) {
  assert(array != NULL);

  DarrHeader *header = (DarrHeader *)array - 1;
  header->size += elementCount;

  if (header->capacity >= header->size)
    return header + 1;

  while (header->capacity < header->size)
    header->capacity *= 2;

  header = (DarrHeader *)realloc(header, sizeof(DarrHeader) + header->capacity * header->elementSize);

  if (header == NULL)
    return NULL;

  return header + 1;
}

void * darrTruncCapacity( void *array ) {
  assert(array != NULL);

  DarrHeader *header = (DarrHeader *)array - 1;

  if (header->capacity == header->size)
    return header + 1;

  header = (DarrHeader *)realloc(header, sizeof(DarrHeader) + header->size);

  if (header == NULL)
    return NULL;

  header->capacity = header->size;

  return header + 1;
}

void * darrToArray( void *array ) {
  assert(array != NULL);

  DarrHeader *header = (DarrHeader *)array - 1;

  void *data = malloc(header->size * header->elementSize);

  if (data == NULL) {
    free(header);
    return NULL;
  }

  memcpy(data, header + 1, header->size * header->elementSize);

  free(header);
  return data;
}

void darrDestroy( void *array ) {
  assert(array != NULL);
  free((DarrHeader *)array - 1);
}

void * darrPush( void *array, const void *elementPtr ) {
  assert(array != NULL);
  assert(elementPtr != NULL);

  if ((array = darrReserve(array, 1)) == NULL)
    return NULL;

  DarrHeader *const header = (DarrHeader *)array - 1;

  memcpy((unsigned char *)(header + 1) + (header->size - 1) * header->elementSize, elementPtr, header->elementSize);

  return header + 1;
}

// darr.c file end