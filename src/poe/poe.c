#include "poe.h"

#include "darr/darr.h"

PoeBool POE_API
poeParseText( FILE *const file, PoeText *const dst ) {
  assert(file != NULL);
  assert(dst != NULL);

  char buffer[2048] = {0};
  const int bufferLength = 2048;

  char *stringBuffer = darrCreate(char, 1);
  if (stringBuffer == NULL)
    return POE_FALSE;

  size_t *stringIndexBuffer = darrCreate(size_t, 0);
  if (stringIndexBuffer == NULL) {
    darrDestroy(stringBuffer);
    return POE_FALSE;
  }

  size_t stringBufferNext = 1;
  size_t stringCount = 0;

  // Initialize string buffer first value (for every string have '\0' in start and in end\, actually)
  stringBuffer[0] = '\0';

  while (fgets(buffer, bufferLength, file) != NULL) {
    const size_t bufferReadSize = strlen(buffer);

    if (bufferReadSize > 0)
      buffer[bufferReadSize - 1] = '\0';

    if ((stringIndexBuffer = darrPush(stringIndexBuffer, &stringBufferNext)) == NULL) {
      darrDestroy(stringBuffer);
      return POE_FALSE;
    }

    stringBuffer = darrReserve(stringBuffer, bufferReadSize);

    if (stringBuffer == NULL) {
      darrDestroy(stringIndexBuffer);
      return POE_FALSE;
    }

    memcpy(stringBuffer + stringBufferNext, buffer, bufferReadSize);

    stringCount += 1;
    stringBufferNext += bufferReadSize;
  }

  if ((stringIndexBuffer = darrPush(stringIndexBuffer, &stringBufferNext)) == NULL) {
    darrDestroy(stringBuffer);
    return POE_FALSE;
  }

  stringBuffer = darrToArray(stringBuffer);

  if (stringBuffer == NULL) {
    darrDestroy(stringIndexBuffer);
    return POE_FALSE;
  }

  PoeString *strings = calloc(stringCount, sizeof(PoeString));

  if (strings == NULL) {
    darrDestroy(stringIndexBuffer);
    darrDestroy(stringBuffer);
    return POE_FALSE;
  }

  for (size_t i = 0; i < stringCount; i++) {
    strings[i].first = stringBuffer + stringIndexBuffer[i];
    strings[i].last = stringBuffer + stringIndexBuffer[i + 1] - 2;
  }
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
    fputs(text->strings[i].first, file);
    fputc('\n', file);
  }
} // poeWriteText function end

void POE_API
poeShuffleText( PoeText *const text, const size_t permutationCount ) {
  PoeString s;

  for (size_t i = 0; i < permutationCount; i++) {
    size_t ia = ((long)rand() + ((long)rand() << 16)) % text->stringCount;
    size_t ib = ((long)rand() + ((long)rand() << 16)) % text->stringCount;

    memcpy(&s                , text->strings + ia, sizeof(PoeString));
    memcpy(text->strings + ia, text->strings + ib, sizeof(PoeString));
    memcpy(text->strings + ib, &s                , sizeof(PoeString));
  }
} // poeShuffleText function end

// file poe.c
