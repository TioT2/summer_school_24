#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poe/poe.h"

/**
 * @brief project main function
 * 
 * @return exit status
 */
int
main( void ) {
  // setup time
  srand((unsigned int)time(NULL));

  PoeBool cond;
  PoeText text = {0};
  FILE *f;

  fopen_s(&f, "test/onegin.txt", "r");
  assert(f != NULL);
  cond = poeParseText(f, &text);
  fclose(f);
  assert(cond);

  {
    PoeGenerator generator = {0};

    cond = poeGeneratorCreate(&text, &generator);
    assert(cond);

    fopen_s(&f, "test_out/gen.txt", "w");
    assert(f != NULL);

    for (size_t i = 0; i < 5; i++) {
      char *st = poeGenerateOneginStanza(&generator);

      if (st != NULL) {
        fprintf(f, "%s\n\n", st);
        free(st);
      }
    }

    fclose(f);

    poeGeneratorDestroy(&generator);
    poeDestroyText(&text);
    return;
  }

  fopen_s(&f, "test_out/out.txt", "w");

  assert(f != NULL);

  poeSortText(&text, poeCompareFromStart);
  poeWriteText(f, &text);
  poeSortText(&text, poeCompareFromEnd);
  poeWriteText(f, &text);
  poeSortText(&text, poeCompareInitialOrder);
  poeWriteText(f, &text);

  fclose(f);

  poeDestroyText(&text);

  return 0;
} // main function end

// main.c file end