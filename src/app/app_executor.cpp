#include "app.h"

#include <signal.h>

static void
appClose( void ) {

} // appClose function end

static void
appSignalHandler( int signal ) {
  const char *signalName;

  switch (signal) {
  case SIGINT  : signalName = "SIGINT"   ; break;
  case SIGILL  : signalName = "SIGILL"   ; break;
  case SIGFPE  : signalName = "SIGFPE"   ; break;
  case SIGSEGV : signalName = "SIGSEGV"  ; break;
  case SIGTERM : signalName = "SIGTERM"  ; break;
  case SIGBREAK: signalName = "SIGBREAK" ; break;
  case SIGABRT : signalName = "SIGABRT"  ; break;
  }

  appClose();
} // appSignalHandler function end


int
appExecutorMain( int argc, const char **argv ) {
  printf("Executor started.\n");

  // setup signal handlers
  signal(SIGINT  , appSignalHandler);
  signal(SIGILL  , appSignalHandler);
  signal(SIGFPE  , appSignalHandler);
  signal(SIGSEGV , appSignalHandler);
  signal(SIGTERM , appSignalHandler);
  signal(SIGBREAK, appSignalHandler);
  signal(SIGABRT , appSignalHandler);

  // End
  appClose();

  return 0;
} // appExecutorMain function end

// app_executor.cpp file end
