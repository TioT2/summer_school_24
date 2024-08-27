#include "poe_generator.h"


/// TODO replace linear search with binary one.

/**
 * @brief last characters getting function
 * 
 * @param string string to get last characters of
 * 
 * @return last character number
 */
static uint32_t
poeGeneratorGetStringLastCharacters( const PoeString *const string ) {
  assert(string != NULL);

  char buffer[4] = {0};
  size_t bufferSize = 0;
  unsigned char *iter = (unsigned char *)string->last;

  while (bufferSize < 3 && *iter != '\0') {
    if (poeCompareCheckCharacterComparability(*iter))
      buffer[bufferSize++] = poeCompareProcessCharacter(*iter);
    iter--;
  }

  return *(uint32_t *)buffer;
} // poeGeneratorGetStringLastCharacters function end

/// Statistics entry representation structure
typedef struct __PoeEndingStatEntry {
  uint32_t last;  ///< last characters
  size_t   count; ///< count of strnigs
} PoeEndingStatEntry;

/**
 * @brief ending statistics getting function
 * 
 * @param text text to get ending statistics of
 * 
 * @return ending statistics dynamic array
 */
static PoeEndingStatEntry *
poeGetEndingStat( const PoeText *const text ) {
  assert(text != NULL);

  PoeEndingStatEntry *entries = darrCreate(sizeof(PoeEndingStatEntry), 0);
  size_t entryCount = 0;

  if (entries == NULL)
    return NULL;

  for (size_t stringIndex = 0; stringIndex < text->stringCount; stringIndex++) {
    uint32_t last = poeGeneratorGetStringLastCharacters(text->strings + stringIndex);
    PoeBool found = POE_FALSE;

    for (size_t entryIndex = 0; entryIndex < entryCount; entryIndex++) {
      if (entries[entryIndex].last == last) {
        entries[entryIndex].count++;
        found = POE_TRUE;
        break;
      }
    }

    // insert ending in statistics
    if (!found) {
      PoeEndingStatEntry entry = {
        .last = last,
        .count = 1,
      };

      if ((entries = darrPush(entries, &entry)) == NULL)
        return NULL;

      entryCount++;
    }
  }

  return entries;
} // poeGetEndingStat function end

PoeBool POE_API
poeGeneratorCreate(
  const PoeText *const text,
  PoeGenerator *const generator
) {
  assert(text != NULL);
  assert(generator != NULL);
  memset(generator, 0, sizeof(PoeGenerator));

  PoeEndingStatEntry *endingStat = poeGetEndingStat(text);
  if (endingStat == NULL)
    return POE_FALSE;
  size_t endingCount = darrGetSize(endingStat);

  PoeEnding *endings = calloc(endingCount, sizeof(PoeEnding));
  if (endings == NULL) {
    darrDestroy(endingStat);
    return POE_FALSE;
  }

  PoeString **stringPool = calloc(text->stringCount, sizeof(PoeString *));
  if (stringPool == NULL) {
    darrDestroy(endingStat);
    free(endings);
    return POE_FALSE;
  }

  PoeString **poolIter = stringPool;

  for (size_t endingIndex = 0; endingIndex < endingCount; endingIndex++) {
    endings[endingIndex].last = endingStat[endingIndex].last;
    endings[endingIndex].strings = poolIter;
    poolIter += endingStat[endingIndex].count;
  }

  for (size_t i = 0; i < text->stringCount; i++) {
    uint32_t last = poeGeneratorGetStringLastCharacters(text->strings + i);

    for (size_t ei = 0; ei < endingCount; ei++)
      if (endings[ei].last == last) {
        endings[ei].strings[endings[ei].stringCount++] = text->strings + i;
        break;
      }
  }

  generator->text        = text;
  generator->stringPool  = stringPool;
  generator->endings     = endings;
  generator->endingCount = endingCount;

  return POE_TRUE;
} // poeGeneratorCreate function end

void POE_API
poeGeneratorDestroy(
  PoeGenerator *const generator
) {
  assert(generator != NULL);

  free(generator->stringPool);
  free(generator->endings);
} // poeGeneratorDestroy function end

void POE_API
poeGeneratorPrint(
  FILE *const file,
  const PoeGenerator *const generator
) {
  assert(file != NULL);
  assert(generator != NULL);

  union {
    uint32_t u32;
    char     arr[5];
  } last = { .arr = {0} };

  for (size_t endingIndex = 0; endingIndex < generator->endingCount; endingIndex++) {
    const PoeEnding *const ending = generator->endings + endingIndex;

    for (size_t i = 0; i < ending->stringCount; i++)
      fprintf(file, "%s\n", ending->strings[i]->first);
    fprintf(file, "\n----------------------------------------------------------------------\n");
  }
} // poePrintEndings function end

// poe_generator.c function end
