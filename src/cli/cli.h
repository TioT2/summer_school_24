#ifndef CLI_H_
#define CLI_H_

#include <sqs/sqs.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

/// @brief Command line parameter iterator utility
typedef struct __CliParameterIterator {
  int           _argc;  // argument count
  const char ** _argv;  // argument values
  int           _index; // index of current argument
} CliParameterIterator;

//----------------------------------------------------------------
//! @brief parameter iterator initialization function
//!
//! @param [in] argc count of command line arguments provided
//! @param [in] argv command line argument values
//! @param [in] iter pointer of iterator to initialize
//----------------------------------------------------------------
void
cliInitParameterIterator(
  const int argc,
  const char **argv,
  CliParameterIterator *const iter
);

//----------------------------------------------------------------
//! @brief next parameter getting function
//!
//! @param [in] this pointer to cli parameter iterator
//! 
//! @return next parameter, returns NULL if no more parameters provided
//----------------------------------------------------------------
const char *
cliParameterIteratorNext( CliParameterIterator *const restrict this );

/// @brief Utility
#define CLI_TO_STRING(t) #t

/// @brief foreground color setting string generation macro
#define CLI_SET_FOREGROUND_COLOR(red, green, blue) "\033[38;2;"CLI_TO_STRING(red)";"CLI_TO_STRING(green)";"CLI_TO_STRING(blue)"m"

/// @brief background color setting string generation macro
#define CLI_SET_BACKGROUND_COLOR(red, green, blue) "\033[48;2;"CLI_TO_STRING(red)";"CLI_TO_STRING(green)";"CLI_TO_STRING(blue)"m"

/// @brief color resetting macro
#define CLI_RESET_COLOR() "\033[39m\033[49m"

//----------------------------------------------------------------
//! @brief stdin from console input clearing function
//----------------------------------------------------------------
void
cliClearStdin( void );

//----------------------------------------------------------------
//! @brief test performing function
//! 
//! @return SQS_TRUE if all tests Ok, SQS_FALSE otherwise.
//----------------------------------------------------------------
SqsBool
cliRunTests( void );

//----------------------------------------------------------------
//! @brief menu running function
//----------------------------------------------------------------
void
cliRunMenu( void );

//----------------------------------------------------------------
//! @brief help running function
//----------------------------------------------------------------
void
cliRunHelp( void );

#endif // !defined(CLI_H_)
