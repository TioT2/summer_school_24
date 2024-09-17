/**
 * @file   poe/poe_core.h
 * @author tiot2
 * @brief  Poem processor core declaration module
 */

#ifndef POE_CORE_H_
#define POE_CORE_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

// dynamic array
#include <darr/darr.h>

/// boolean type
typedef int PoeBool;

/// true definition
#define POE_TRUE 1

/// false definition
#define POE_FALSE 0

/// calling convention
#define POE_API __cdecl

// Common status entries definition macro
#define POE_DEFINE_COMMON_STATUS(PREFIX)         \
  PREFIX ## _OK        = 0, /* Succeeded      */ \
  PREFIX ## _BAD_ALLOC = 1, /* Bad allocation */ \

/// Status representation structure
typedef enum __PoeStatus {
  POE_DEFINE_COMMON_STATUS(POE_STATUS)
} PoeStatus;

/// Status checking macro definition
#define POE_CHECK(expr) ((POE_STATUS_OK) == (PoeStatus)(expr))

/// string representation structure
typedef struct __PoeString {
  char *begin; ///< first character pointer
  char *end;   ///< string end (points to '\0' string character)
} PoeString;

/// text representation structure
typedef struct __PoeText {
  char      * stringBuffer; ///< string bulk allocation
  PoeString * strings;      ///< text string pointer
  size_t      stringCount;  ///< count of text strings
} PoeText;


/**
 * @brief text parsing function
 * 
 * @param file    file with text data
 * @param dstText parsed text
 * 
 * @return POE_TRUE if parsed successfully, POE_FALSE otherwise
 */
PoeStatus POE_API
poeParseText( FILE *file, PoeText *dst );

/**
 * @brief text writing function
 * 
 * @param file file to write text to
 * @param text text to write
 */
void POE_API
poeWriteText( FILE *file, const PoeText *text );

/**
 * @brief text destructor
 * 
 * @param text text to destroy
 */
void POE_API
poeDestroyText( PoeText *text );

#endif // !defined(POE_CORE_H_)

// poe_core.h file end
