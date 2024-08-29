/**
 * @file   poe/poe_generator.h
 * @author tiot2
 * @brief  Poem processor new text generator declaration module
 */

#ifndef POE_GENERATOR2_H_
#define POE_GENERATOR2_H_

#include "poe_sort.h"

/// Second-generation text generator
typedef struct __PoeGenerator2 {
  const PoeText *text; ///< Text
  PoeString *strings;  ///< Own strings
} PoeGenerator2;

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
);

/**
 * @brief stanza generation function
 * 
 * @param generator    generator to generate stanza in
 * @param stanzaBuffer buffer to write stanza lines in
 * 
 * @return true if generated, false otherwise
 */
PoeBool POE_API
poeGenerateOneginStanza2( const PoeGenerator2 *const generator, const PoeString **stanzaBuffer );

/**
 * @brief string generator initialization function
 * 
 * @param generator generator to deinitialize
 */
void POE_API
poeDestroyGenerator2(
  PoeGenerator2 *const generator
);

#endif // !defined(POE_GENERATOR2_H_)

// poe_generator2.h file end
