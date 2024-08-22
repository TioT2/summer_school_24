#include "app.h"

int
appDaemonMain( int argc, const char **argv ) {
  printf("DAEMON STARTED\n");

  HANDLE commandPipe = CreateNamedPipe(
    APP_DAEMON_COMMAND_PIPE,
    PIPE_ACCESS_INBOUND,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    512,
    512,
    5000,
    NULL
  );

  if (commandPipe == INVALID_HANDLE_VALUE) {
    printf("Failed to open input daemon pipe. ");
    appPrintWinapiError(stdout, GetLastError());
    printf("\n");
    return 1;
  }

  while (TRUE) {
    printf("Waiting client...\n");
    int connected = ConnectNamedPipe(commandPipe, NULL);

    if (!connected) {
      printf("Error connecting to command pipe\n");
      continue;
    }

    char inputBuffer[256] = {0};
    DWORD bytesRead = 0;

    while (TRUE) {
      if (!ReadFile(commandPipe, inputBuffer, sizeof(inputBuffer), &bytesRead, NULL)) {
        break;
      }

      if (inputBuffer[0] == '#') {
        if (strcmp(inputBuffer, "#msg quit") == 0)
          break;

        printf("  Unknown client command: %s\n", inputBuffer);
      } else {
        printf("  %s\n", inputBuffer);
      }
    }

    printf("Client disconnected.\n");
    DisconnectNamedPipe(commandPipe);
  }

  CloseHandle(commandPipe);

  return 0;
} // appDaemonMain function end

// app_daemon.cpp file end
