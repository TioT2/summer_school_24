/**
 * @file   poe/poe_generator.h
 * @author tiot2
 * @brief  Poem processor new text generator implementation module
 */

#include "poe_generator2.h"

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
  unsigned char *iter = (unsigned char *)string->end - 1;

  while (bufferSize < 3 && *iter != '\0') {
    if (poeCompareCheckCharacterComparability(*iter))
      buffer[bufferSize++] = poeCompareProcessCharacter(*iter);
    iter--;
  }

  return *(uint32_t *)buffer;
} // poeGeneratorGetStringLastCharacters function end

/**
 * @brief generator constructor
 * 
 * @param text      text to generate poems based on
 * @param generator generator, actually
 * 
 * @return POE_TRUE if initialization succeeded, POE_FALSE otherwise
 */
PoeBool POE_API
poeCreateGenerator2(
  const PoeText *const text,
  PoeGenerator2 *const generator
) {
  assert(text != NULL);
  assert(generator != NULL);

  PoeString *strings = (PoeString *)calloc(text->stringCount, sizeof(PoeString));

  if (strings == NULL)
    return POE_FALSE;

  memcpy(strings, text->strings, text->stringCount * sizeof(PoeString));
  /// sort strings
  qsort_s(strings, text->stringCount, sizeof(PoeString), poeStdCompareWrapper, poeCompareFromEnd);

  generator->text = text;
  generator->strings = strings;

  return POE_TRUE;
} // poeCreateGenerator2 function end

PoeBool POE_API
poeGenerateOneginStanza2( const PoeGenerator2 *const generator, const PoeString **stanzaBuffer ) {
  assert(generator != NULL);

  size_t baseIndices[7] = { 0 };

  for (size_t i = 0; i < 7; i++) {
    const PoeString *str = generator->strings + baseIndices[i];

    // trying to ignore empty strings
    do {
      baseIndices[i] = rand() % (generator->text->stringCount - 30) + 15;
      str = generator->strings + baseIndices[i];
    } while (str->begin == str->end);
  }

  const PoeString *lines[14] = {NULL};

  for (size_t baseIndexIter = 0; baseIndexIter < 7; baseIndexIter++) {
    size_t baseIndex = baseIndices[baseIndexIter];

    const PoeString *first = generator->strings + baseIndex;
    const PoeString *second = NULL;

    uint32_t firstLastChars = poeGeneratorGetStringLastCharacters(generator->strings + baseIndex);

    size_t i = 0;
    for (i = 14; i > 0; i--)
      if (poeGeneratorGetStringLastCharacters(generator->strings + baseIndex + i) == firstLastChars) {
        second = generator->strings + baseIndex + i;
        break;
      } else if (poeGeneratorGetStringLastCharacters(generator->strings + baseIndex - i) == firstLastChars) {
        second = generator->strings + baseIndex - i;
        break;
      }

    if (second == NULL)
      return POE_FALSE;

    lines[baseIndexIter * 2 + 0] = first;
    lines[baseIndexIter * 2 + 1] = second;
  }

  stanzaBuffer[ 0] = lines[ 0];
  stanzaBuffer[ 1] = lines[ 2];
  stanzaBuffer[ 2] = lines[ 1];
  stanzaBuffer[ 3] = lines[ 3];

  stanzaBuffer[ 4] = lines[ 4];
  stanzaBuffer[ 5] = lines[ 5];
  stanzaBuffer[ 6] = lines[ 6];
  stanzaBuffer[ 7] = lines[ 7];

  stanzaBuffer[ 8] = lines[ 8];
  stanzaBuffer[ 9] = lines[10];
  stanzaBuffer[10] = lines[11];
  stanzaBuffer[11] = lines[ 9];

  stanzaBuffer[12] = lines[12];
  stanzaBuffer[13] = lines[13];

  return POE_TRUE;
} // poeGenerateOneginStanza2 function end

void POE_API
poeDestroyGenerator2(
  PoeGenerator2 *const generator
) {
  assert(generator != NULL);

  free(generator->strings);
} // poeCreateGenerator function end

// poe_generator2.cpp file end
