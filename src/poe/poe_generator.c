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

struct __PoeResultOf_poeGeneratorGetRandPair {
  size_t first;
  size_t second;
}
poeGeneratorGetRandPair( const size_t mod ) {
  assert(mod >= 2);

  struct __PoeResultOf_poeGeneratorGetRandPair out = {
    .first = rand() % mod,
    .second = rand() % mod,
  };

  while (out.second == out.first)
    out.second = rand() % mod;

  return out;
}

char * POE_API
poeGenerateOneginStanza( const PoeGenerator *const generator ) {
  assert(generator != NULL);

  const PoeEnding * endings[7] = {NULL};

  for (size_t i = 0; i < 7; i++) {
    for (;;) {
      endings[i] = generator->endings + rand() % generator->endingCount;

      if (endings[i]->stringCount < 2)
        continue;
      break;
    }
  }

  struct __PoeResultOf_poeGeneratorGetRandPair randPair1, randPair2;

  const PoeString * lines[14] = {0};

  randPair1 = poeGeneratorGetRandPair(endings[0]->stringCount);
  randPair2 = poeGeneratorGetRandPair(endings[1]->stringCount);

  lines[ 0] = endings[0]->strings[randPair1.first ];
  lines[ 1] = endings[1]->strings[randPair2.first ];
  lines[ 2] = endings[0]->strings[randPair1.second];
  lines[ 3] = endings[1]->strings[randPair2.second];

  randPair1 = poeGeneratorGetRandPair(endings[2]->stringCount);
  randPair2 = poeGeneratorGetRandPair(endings[3]->stringCount);

  lines[ 4] = endings[2]->strings[randPair1.first ];
  lines[ 5] = endings[2]->strings[randPair1.second];
  lines[ 6] = endings[3]->strings[randPair2.first ];
  lines[ 7] = endings[3]->strings[randPair2.second];

  randPair1 = poeGeneratorGetRandPair(endings[4]->stringCount);
  randPair2 = poeGeneratorGetRandPair(endings[5]->stringCount);

  lines[ 8] = endings[4]->strings[randPair1.first ];
  lines[ 9] = endings[5]->strings[randPair2.first ];
  lines[10] = endings[5]->strings[randPair2.second];
  lines[11] = endings[4]->strings[randPair1.second];

  randPair1 = poeGeneratorGetRandPair(endings[6]->stringCount);

  lines[12] = endings[6]->strings[randPair1.first ];
  lines[13] = endings[6]->strings[randPair1.second];

  size_t totalLength = 0;

  for (size_t i = 0; i < sizeof(lines) / sizeof(lines[0]); i++)
    totalLength += lines[i]->last + 2 - lines[i]->first;

  char *buffer = calloc(totalLength, sizeof(char));

  if (buffer == NULL)
    return NULL;

  char *iter = buffer;

  for (size_t i = 0; i < sizeof(lines) / sizeof(lines[0]); i++) {
    size_t size = lines[i]->last + 1 - lines[i]->first;
    memcpy(iter, lines[i]->first, size);
    iter += size;

    *iter++ = '\n';
  }

  iter[-1] = '\0';

  return buffer;
} // poeGenerateOneginStanza function end

void POE_API
poeGeneratorPrint(
  FILE *const file,
  const PoeGenerator *const generator
) {
  assert(file != NULL);
  assert(generator != NULL);

  for (size_t endingIndex = 0; endingIndex < generator->endingCount; endingIndex++) {
    const PoeEnding *const ending = generator->endings + endingIndex;

    for (size_t i = 0; i < ending->stringCount; i++)
      fprintf(file, "%s\n", ending->strings[i]->first);
    fprintf(file, "\n----------------------------------------------------------------------\n");
  }
} // poePrintEndings function end

// poe_generator.c function end
