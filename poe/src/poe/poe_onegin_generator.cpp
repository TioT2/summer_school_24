/**
 * @file   poe/poe_onegin_generator.h
 * @author tiot2
 * @brief  Poem processor Onegin-rhytm-based generator implementation module
 */

#include "poe_onegin_generator.h"

PoeOneginGeneratorStatus POE_API
poeCreateOneginGenerator(
  const PoeText *const text,
  PoeOneginGenerator *const generator
) {
  assert(text != NULL);
  assert(generator != NULL);

  const PoeString **stanzaStartLines = (const PoeString **)darrCreate(sizeof(PoeString *), 0);

  for (size_t stringIndex = 0; stringIndex < text->stringCount - 15; stringIndex++) {
    const PoeString *current = text->strings + stringIndex;

    if (current->begin != current->end) {
      continue;
    }

    PoeBool isStanza = POE_TRUE;
    for (size_t i = 1; i < 15; i++) {
      if (current[i].begin == current[i].end) {
        isStanza = POE_FALSE;
        break;
      }
    }

    if (!isStanza)
      continue;

    // check for line is empty or not
    if (current[16].begin != current[16].end)
      continue;

    current++;
    if ((stanzaStartLines = (const PoeString **)darrPush(stanzaStartLines, &current)) == NULL)
      return POE_ONEGIN_GENERATOR_STATUS_BAD_ALLOC;
  }

  size_t stanzaCount = darrGetSize(stanzaStartLines);

  size_t stringPairCount = stanzaCount * POE_ONEGIN_BUCKET_COUNT;
  PoeOneginStringPair *stringPairBuffer = (PoeOneginStringPair *)calloc(stringPairCount, sizeof(PoeOneginStringPair));

  for (size_t i = 0; i < POE_ONEGIN_BUCKET_COUNT; i++) {
    generator->buckets[i].stringPairSet = stringPairBuffer + stanzaCount * i;
    generator->buckets[i].stringPairCount = 0;
  }

  struct {
    size_t
      first,  ///< first string index
      second; ///< second string index
  } stringBucketIndices[7] = {
    { 0,  2}, { 1,  3}, { 4,  5},
    { 6,  7}, { 8, 11}, { 9, 10},
    {12, 13},
  };

  for (size_t i = 0; i < stanzaCount; i++) {
    const PoeString *stanzaStart = stanzaStartLines[i];

    for (size_t bucketIndex = 0; bucketIndex < POE_ONEGIN_BUCKET_COUNT; bucketIndex++) {
      PoeOneginBucket *bucket = generator->buckets + bucketIndex;

      bucket->stringPairSet[bucket->stringPairCount].first  = stanzaStart[stringBucketIndices[bucketIndex].first ];
      bucket->stringPairSet[bucket->stringPairCount].second = stanzaStart[stringBucketIndices[bucketIndex].second];

      bucket->stringPairCount++;
    }
  }

  darrDestroy(stanzaStartLines);

  generator->stringPairBuffer = stringPairBuffer;
  generator->stringPairCount = stringPairCount;
  generator->text = text;

  return POE_ONEGIN_GENERATOR_STATUS_OK;
} // poeCreateOneginGenerator function end

PoeOneginGeneratorStatus POE_API
poeOneginGenerateStanza(
  const PoeOneginGenerator *const generator,
  const PoeString **stanzaBuffer
) {
  assert(generator != NULL);
  assert(stanzaBuffer != NULL);

  const PoeOneginStringPair *pairs[POE_ONEGIN_BUCKET_COUNT];
  
  for (size_t i = 0; i < POE_ONEGIN_BUCKET_COUNT; i++)
    pairs[i] = generator->buckets[i].stringPairSet + rand() % generator->buckets[i].stringPairCount;

  stanzaBuffer[ 0] = &pairs[0]->first ;
  stanzaBuffer[ 1] = &pairs[1]->first ;
  stanzaBuffer[ 2] = &pairs[0]->second;
  stanzaBuffer[ 3] = &pairs[1]->second;

  stanzaBuffer[ 4] = &pairs[2]->first ;
  stanzaBuffer[ 5] = &pairs[2]->second;
  stanzaBuffer[ 6] = &pairs[3]->first ;
  stanzaBuffer[ 7] = &pairs[3]->second;

  stanzaBuffer[ 8] = &pairs[4]->first ;
  stanzaBuffer[ 9] = &pairs[5]->first ;
  stanzaBuffer[10] = &pairs[5]->second;
  stanzaBuffer[11] = &pairs[4]->second;

  stanzaBuffer[12] = &pairs[6]->first;
  stanzaBuffer[13] = &pairs[6]->second;

  return POE_ONEGIN_GENERATOR_STATUS_OK;
} // poeOneginGenerateStanza function end

void POE_API
poeDestroyOneginGenerator(
  PoeOneginGenerator *const generator
) {
  assert(generator != NULL);

  free(generator->stringPairBuffer);
} // poeDestroyOneginGenerator function end

// poe_onegin_generator.cpp file end
