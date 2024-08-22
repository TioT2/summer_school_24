#include "app.h"

int
appClientMain( int argc, const char **argv ) {
  HANDLE daemonCommandFile = CreateFile(
    APP_DAEMON_COMMAND_PIPE,
    GENERIC_WRITE,
    FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL
  );

  if (daemonCommandFile == INVALID_HANDLE_VALUE) {
    printf(
      "FATAL ERORR:"
      "Can't open daemon handle\n"
    );
    return 0;
  }

  while (TRUE) {
    char buffer[256] = {0};

    printf(">>> ");
    
    for (int i = 0; i < sizeof(buffer) - 1; i++) {
      char c = (char)getchar();
      if (c == '\n') {
        buffer[i] = '\0';
        break;
      }
      buffer[i] = c;
    }

    if (buffer[0] == '#') {
      if (strcmp(buffer, "#exit") == 0) {
        const char *message = "#msg quit";

        WriteFile(daemonCommandFile, message, (DWORD)strlen(message), NULL, NULL);
        break;
      }
      printf("Unknow command: %s\n", buffer);
    } else {
      WriteFile(daemonCommandFile, buffer, (DWORD)strlen(buffer) + 1, NULL, NULL);
    }
  }

  CloseHandle(daemonCommandFile);

  return 0;
} // appClientMain function end

// app_client.cpp file end