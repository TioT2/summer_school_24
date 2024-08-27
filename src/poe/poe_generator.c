#include "poe_generator.h"

PoeBool POE_API
poeGeneartorCreate(
  const PoeText *const text,
  PoeGenerator *const generator
) {
  assert(text != NULL);
  assert(generator != NULL);
  memset(generator, 0, sizeof(PoeGenerator));

  generator->stringBuffer = calloc(text->stringCount, sizeof(PoeString));
  if (generator->stringBuffer == NULL)
    return POE_FALSE;

  return POE_FALSE;
} // poeGeneratorCreate function end

void POE_API
poeGeneratorDestroy(
  PoeGenerator *const generator
) {
  assert(generator != NULL);

  generator->text->strings;

  free(generator->stringBuffer);
  free(generator->endingPool);
} // poeGeneratorDestroy function end

// poe_generator.c function end
