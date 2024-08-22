#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "app/app.h"

/// @brief Utility
#define CLI_TO_STRING(t) #t

/// @brief foreground color setting string generation macro
#define CLI_SET_FOREGROUND_COLOR(red, green, blue) "\033[38;2;" CLI_TO_STRING(red) ";" CLI_TO_STRING(green) ";" CLI_TO_STRING(blue) "m"

/// @brief background color setting string generation macro
#define CLI_SET_BACKGROUND_COLOR(red, green, blue) "\033[48;2;" CLI_TO_STRING(red) ";" CLI_TO_STRING(green) ";" CLI_TO_STRING(blue) "m"

/// @brief color resetting macro
#define CLI_RESET_COLOR() "\033[39m\033[49m"

typedef enum __AppExecutionMode {
  APP_EXECUTION_MODE_UNDEFINED, // Undefined mode, resolves to client
  APP_EXECUTION_MODE_DAEMON,    // Run daemon. Only one daemon instance should run on single device
  APP_EXECUTION_MODE_CLIENT,    // Client - interface
  APP_EXECUTION_MODE_EXECUTOR,  // Executor of daemon commands
} AppExecutionMode;

//----------------------------------------------------------------
//! @brief project entry point
//!
//! @param [in] argc count of console parameters
//! @param [in] argv parameter strings
//! 
//! @return exit status
//----------------------------------------------------------------
int
main(
  int argc,
  const char **argv
) {
  system(" ");

  AppExecutionMode executionMode = APP_EXECUTION_MODE_UNDEFINED;

  // define execution modes
  const static struct __ExecModeDesc {
    const char *key;
    AppExecutionMode mode;
  } modes[] = {
    {"-d", APP_EXECUTION_MODE_DAEMON},
    {"-c", APP_EXECUTION_MODE_CLIENT},
    {"-e", APP_EXECUTION_MODE_EXECUTOR},
  };

  if (argc > 1) {
    const char *key = argv[1];

    for (int modeI = 0, modeCount = sizeof(modes) / sizeof(modes[0]); modeI < modeCount; modeI++) {
      struct __ExecModeDesc mode = modes[modeI];

      if (strcmp(mode.key, key) == 0) {
        executionMode = mode.mode;
        break;
      }
    }
  }

  if (executionMode == APP_EXECUTION_MODE_UNDEFINED)
    executionMode = APP_EXECUTION_MODE_CLIENT;

  // custom argc/argv
  int          argCount = 1;
  const char **argValues = argv;

  if (argc >= 2) {
    argCount = argc - 2;
    argValues = argv + 2;
  }

  switch (executionMode) {
  case APP_EXECUTION_MODE_DAEMON   : return appDaemonMain  (argCount, argValues);
  case APP_EXECUTION_MODE_CLIENT   : return appClientMain  (argCount, argValues);
  case APP_EXECUTION_MODE_EXECUTOR : return appExecutorMain(argCount, argValues);
  default                          : return 1;
  }
} // main function end

// main.cpp file end
