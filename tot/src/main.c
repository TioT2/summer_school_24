#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int TotBool;
#define TOT_TRUE  1
#define TOT_FALSE 0

typedef struct __TotEntry {
  unsigned int count;
} TotEntry;

typedef struct __TotTable {
  size_t     size;
  TotEntry * entries;
} TotTable;

TotBool
totTableInit( TotTable *const table, size_t size ) {
  size_t length = size * (size - 1) / 2;
  TotEntry *entries = calloc(length, sizeof(TotEntry));

  if (entries == NULL)
    return TOT_FALSE;

  table->entries = entries;
  table->size = size;

  return TOT_TRUE;
} // totTableInit function end

void
totTableClose( TotTable *const table ) {
  free(table->entries);
} // totTableClose function end

TotBool
totTableGet( const TotTable *const table, size_t x, size_t y, TotEntry *const dst ) {
  if (x + y > table->size)
    return TOT_FALSE;
  if (y < x) {
    size_t tmp = y;
    y = x;
    x = tmp;
  }

  memcpy(dst, table->entries + y * (y - 1) / 2 + x, sizeof(TotEntry));
  return TOT_TRUE;
}

TotBool
totTableSet( TotTable *const table, size_t x, size_t y, const TotEntry *entry ) {
  if (x == y || x >= table->size || y >= table->size)
    return TOT_FALSE;
  if (x > y) {
    size_t tmp = y;
    y = x;
    x = tmp;
  }

  memcpy(table->entries + y * (y - 1) / 2 + x, entry, sizeof(TotEntry));
  return TOT_TRUE;
}

void
totTablePrint( TotTable *const table, FILE *const file ) {
  size_t rowLength = 0;

  for (size_t y = 0; y < table->size; y++) {
    for (size_t x = 0; x < y; x++)
      fprintf(file, "%4d ", table->entries[y * (y - 1) / 2 + x].count);
    fputc('\n', file);
  }
}

int main( void ) {
  TotTable t = {0};
  TotEntry d = {0};

  totTableInit(&t, 4);

  d.count = 12; totTableSet(&t, 0, 1, &d);
  d.count = 13; totTableSet(&t, 0, 2, &d);
  d.count = 14; totTableSet(&t, 0, 3, &d);

  d.count = 23; totTableSet(&t, 1, 2, &d);
  d.count = 24; totTableSet(&t, 1, 3, &d);

  d.count = 34; totTableSet(&t, 2, 3, &d);

  totTablePrint(&t, stdout);

  totTableClose(&t);

  return 0;
} // main function end

// main.c file end
