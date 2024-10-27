#include "app.h"

//----------------------------------------------------------------
//! @brief help entry point
//!
//! @param [in] argc count of input parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
appHelpMain( int, const char ** ) {
  printf(
    "KEYS: \n"
    "  -h   Show this menu\n"
    "  -d   Run daemon\n"
    "  -c   Run client (running daemon instance required)\n"
    "  -e   Run executor\n"
  );

  return 0;
} // appHelpMain function end

// app_help.cpp file end