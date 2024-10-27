#include "app.h"

int
appPrintWinapiError( FILE *const file, DWORD error ) {
  LPSTR messageBuffer = nullptr;
  int size = (int)FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    error,
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
    (LPSTR)&messageBuffer,
    0,
    NULL
  );
  fwrite(messageBuffer, 1, size, file);
  LocalFree(messageBuffer);

  return size;
}
