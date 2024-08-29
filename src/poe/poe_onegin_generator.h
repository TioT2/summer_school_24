/**
 * @file   poe/poe_onegin_generator.h
 * @author tiot2
 * @brief  Poem processor Onegin-rhytm-based generator declaration module
 */

#ifndef POE_ONEGIN_GENERATOR_H_
#define POE_ONEGIN_GENERATOR_H_

#include "poe_compare.h"

/// just string pair, actually
typedef struct __PoeOneginStringPair {
  PoeString first;  ///< first string
  PoeString second; ///< second string
} PoeOneginStringPair;

/// generator bucket
typedef struct __PoeOneginBucket {
  PoeOneginStringPair * stringPairSet;   ///< string set
  size_t                stringPairCount; ///< count of strings in set
} PoeOneginBucket;

/// count of buckets
#define POE_ONEGIN_BUCKET_COUNT 7

/// text generator
typedef struct __PoeOneginGenerator {
  const PoeText       * text;                             ///< text strings string pair buffer refers to
  PoeOneginStringPair * stringPairBuffer;                 ///< string pair bulk allocation
  size_t                stringPairCount;                  ///< pair count
  PoeOneginBucket       buckets[POE_ONEGIN_BUCKET_COUNT]; ///< string buckets
} PoeOneginGenerator;

/**
 * @brief generator create function
 * 
 * @param text      text to generate stanza from
 * @param generator generator to generate text by
 * 
 * @note text must be built from 14-line Onegin stanzas
 * 
 * @return POE_TRUE if success, POE_FALSE otherwise
 */
PoeBool POE_API
poeCreateOneginGenerator(
  const PoeText *text,
  PoeOneginGenerator *generator
);

/**
 * @brief stanza generation function
 * 
 * @param generator    generator to generate stanza by
 * @param stanzaBuffer buffer to write answer (note: minimal accepted size of buffer is 14)
 * 
 * @return POE_TRUE if generated, POE_FALSE otherwise
 */
PoeBool POE_API
poeOneginGenerateStanza(
  const PoeOneginGenerator *generator,
  const PoeString **stanzaBuffer
);

/**
 * @brief generator destroy function
 * 
 * @param generator generator to destroy
 */
void POE_API
poeDestroyOneginGenerator(
  PoeOneginGenerator *generator
);

#endif // !defined(POE_ONEGIN_GENERATOR_H_)

// poe_onegin_generator.h file end
