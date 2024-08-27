#ifndef POE_CORE_H_
#define POE_CORE_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include <darr/darr.h>

/// Boolean type
typedef int PoeBool;

/// TRUE
#define POE_TRUE 1

/// FALSE
#define POE_FALSE 0

/// Calling convention
#define POE_API __cdecl

typedef struct __PoeString {
  char *first; // first character pointer
  char *last;  // last character pointer
} PoeString;

/// Text representation structure
typedef struct __PoeText {
  char      * stringBuffer; ///< string bulk allocation
  PoeString * strings;      ///< text string pointer
  size_t      stringCount;  ///< count of text strings
} PoeText;

/**
 * @brief Text parsing function
 * 
 * @param file    file with text data
 * @param dstText parsed text
 * 
 * @return POE_TRUE if parsed successfully, POE_FALSE otherwise
 */
PoeBool POE_API
poeParseText( FILE *const file, PoeText *const dst );

/**
 * @brief Text writing function
 * 
 * @param file file to write text to
 * @param text text to write
 */
void POE_API
poeWriteText( FILE *const file, const PoeText *const text );

/**
 * @brief text destructor
 * 
 * @param text text to destroy
 */
void POE_API
poeDestroyText( PoeText *const text );

#endif // !defined(POE_CORE_H_)

// poe_core.h file end
