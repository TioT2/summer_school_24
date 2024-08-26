#include "poe.h"

#include "darr/darr.h"

PoeBool POE_API
poeParseText( FILE *const file, PoeText *const dst ) {
  assert(file != NULL);
  assert(dst != NULL);

  char buffer[2048] = {0};
  const int bufferLength = 2048;

  char *stringBuffer = darrCreate(char, 0);
  size_t *stringIndexBuffer = darrCreate(size_t, 0);

  size_t stringBufferNext = 0;
  size_t stringCount = 0;

  while (fgets(buffer, bufferLength, file) != NULL) {
    size_t bufferReadSize = strlen(buffer);

    if (bufferReadSize > 0)
      buffer[bufferReadSize - 1] = '\0';

    stringBuffer = darrReserve(stringBuffer, bufferReadSize);

    if (stringBuffer == NULL) {
      darrDestroy(stringIndexBuffer);
      return POE_FALSE;
    }

    stringIndexBuffer = darrReserve(stringIndexBuffer, 1);

    if (stringIndexBuffer == NULL) {
      darrDestroy(stringBuffer);
      return POE_FALSE;
    }

    memcpy(stringBuffer + stringBufferNext, buffer, bufferReadSize);

    stringIndexBuffer[stringCount] = stringBufferNext;

    stringCount += 1;
    stringBufferNext += bufferReadSize;
  }

  stringBuffer = darrToArray(stringBuffer);

  if (stringBuffer == NULL) {
    darrDestroy(stringIndexBuffer);
    return POE_FALSE;
  }

  const char **strings = calloc(stringCount, sizeof(char *));

  if (strings == NULL) {
    darrDestroy(stringIndexBuffer);
    darrDestroy(stringBuffer);
    return POE_FALSE;
  }

  for (size_t i = 0; i < stringCount; i++)
    strings[i] = stringBuffer + stringIndexBuffer[i];
  darrDestroy(stringIndexBuffer);

  dst->stringBuffer = stringBuffer;
  dst->strings = strings;
  dst->stringCount = stringCount;

  return POE_TRUE;
} // poeParseText function end

void POE_API
poeDestroyText( PoeText *const text ) {
  assert(text != NULL);

  free(text->stringBuffer);
  free(text->strings);
} // poeDestroyText function end

void POE_API
poeWriteText( FILE *const file, const PoeText *const text ) {
  assert(file != NULL);
  assert(text != NULL);

  for (size_t i = 0; i < text->stringCount; i++) {
    fputs(text->strings[i], file);
    fputc('\n', file);
  }
} // poeWriteText function end

// file poe.c
