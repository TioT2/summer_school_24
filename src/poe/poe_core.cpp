/**
* 
 * @file   poe/poe_core.cpp
 * @author tiot2
 * @brief  Poem processor core implementation module
 */

#include "poe.h"

PoeStatus POE_API
poeParseText( FILE *const file, PoeText *const dst ) {
  assert(file != NULL);
  assert(dst != NULL);

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // with starting and ending \0
  char *stringBuffer = (char *)calloc(size + 2, 1);
  if (stringBuffer == NULL)
    return POE_STATUS_BAD_ALLOC;
  fread(stringBuffer + 1, 1, size, file);

  char *writer = stringBuffer + 1;
  const char *reader = stringBuffer + 1;
  char *readerEnd = stringBuffer + size + 1;

  while (reader < readerEnd) {
    while (*reader == '\r')
      reader++;
    *writer++ = *reader++;
  }

  memset(writer, 0, readerEnd - writer);

  size_t stringCount = 1;
  for (const char *t = stringBuffer; t < writer; t++)
    stringCount += (*t == '\n');
  PoeString *strings = (PoeString *)calloc(stringCount, sizeof(PoeString));

  if (strings == NULL) {
    free(stringBuffer);
    return POE_STATUS_BAD_ALLOC;
  }

  {
    PoeString *stringIter = strings;
    char *charIter = stringBuffer + 1;
    char *const charIterEnd = writer;
    
    strings[0].begin = charIter;
    strings[stringCount - 1].end = writer;

    while (charIter < charIterEnd) {
      if (*charIter == '\n') {
        stringIter->end = charIter;
        (stringIter + 1)->begin = charIter + 1;
        *charIter = '\0';

        stringIter++;
      }
      charIter++;
    }
  }

  dst->stringBuffer = stringBuffer;
  dst->stringCount = stringCount;
  dst->strings = strings;

  return POE_STATUS_OK;
} // poeParseText2 function end

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
    fputs(text->strings[i].begin, file);
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

// poe_core.cpp file end
