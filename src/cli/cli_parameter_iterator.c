#include "cli.h"

//----------------------------------------------------------------
//! @brief parameter iterator initialization function
//!
//! @param [in] argc count of command line arguments provided
//! @param [in] argv command line argument values
//! @param [in] iter pointer of iterator to initialize
//----------------------------------------------------------------
void cliInitParameterIterator( const int argc, const char **argv, CliParameterIterator *const iter ) {
  assert(argv != NULL);
  assert(iter != NULL);

  iter->_argc = argc;
  iter->_argv = argv;
  iter->_index = 0;
} // cliInitParameterIterator function end

//----------------------------------------------------------------
//! @brief next parameter getting function
//!
//! @param [in] this pointer to cli parameter iterator
//! 
//! @return next parameter, returns NULL if no more parameters provided
//----------------------------------------------------------------
const char * cliParameterIteratorNext( CliParameterIterator *const restrict this ) {
  if (this->_index >= this->_argc)
    return NULL;
  return this->_argv[this->_index++];
} // cliParameterIteratorNext function end
