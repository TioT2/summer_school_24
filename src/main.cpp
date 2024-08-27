#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "poe/poe.h"

/**
 * @brief string from stdout getting function
 * 
 * @param buffer     buffer to write string to
 * @param bufferSize size of buffer allowed to write
 * 
 * @return count of characters written
 */
size_t
cliGetString( char *const buffer, const size_t bufferSize ) {
  assert(buffer != NULL);

  char *bptr = buffer;
  char *const eptr = buffer + bufferSize - 1;

  while (bptr < eptr) {
    char c = (char)getchar();
    if (c == '\n')
      break;
    *bptr++ = c;
  }

  *bptr = '\0';

  return bptr - buffer;
} // cliGetString function end

/**
 * @brief project main function
 * 
 * @return exit status
 */
int
main( void ) {
  // setup time
  srand((unsigned int)time(NULL));
  system("chcp 1251");
  system("cls");

  PoeText text = {0};
  PoeBool textIsInit = POE_FALSE;
  PoeGenerator generator = {0};
  PoeBool generatorIsInit = POE_FALSE;

  FILE *file;

  const char *commandLoad = "load";
  const char *commandGenStanza = "stanza";
  const char *commandHelp = "help";
  const char *commandSort = "sort";
  const char *commandSortInitial = "initial";
  const char *commandSortForward = "forward";
  const char *commandSortReverse = "reverse";
  const char *commandWrite = "write";
  const char *commandQuit = "quit";

  char buffer[512] = {0};

  PoeBool doContinue = POE_TRUE;

  while (doContinue) {
    printf(">>> ");

    cliGetString(buffer, sizeof(buffer));

    if (buffer[0] == '!') {
      system(buffer + 1);
      continue;
    }

    const char *next = "";

    // find second command part
    {
      char *n = strchr(buffer, ' ');

      if (n != NULL) {
        *n = '\0';
        next = n + 1;
      }
    }

    if (strcmp(buffer, commandLoad) == 0) {
      if (generatorIsInit) {
        // generator deinitialization
        poeDestroyGenerator(&generator);
        generatorIsInit = POE_FALSE;
      }

      fopen_s(&file, next, "r");

      if (file == NULL) {
        printf("    can't open file \'%s\'\n", next);
        continue;
      }

      if (poeParseText(file, &text))
        textIsInit = POE_TRUE;
      else
        printf("    error during text file parsing occured\n");

      fclose(file);

      continue;
    }

    if (strcmp(buffer, commandHelp) == 0) {
      printf("    load file              %s <file name>\n", commandLoad);
      printf("    generate stanza        %s\n", commandGenStanza);
      printf("    sort with comparator   %s <\'%s\'|\'%s\'|\'%s\'>\n", commandSort, commandSortInitial, commandSortForward, commandSortReverse);
      printf("    write to file          %s <file name>\n", commandWrite);
      printf("\n");
      printf("    show this menu         %s\n", commandHelp);
      printf("    quit from program      %s\n", commandQuit);
      printf("    execute cmd command    !<command>\n");
      continue;
    }

    if (strcmp(buffer, commandGenStanza) == 0) {
      if (!textIsInit) {
        printf("    no text to generate stanza from\n");
        continue;
      }

      if (!generatorIsInit)
        if (poeCreateGenerator(&text, &generator)) {
          generatorIsInit = POE_TRUE;
        } else {
          printf("    error during text generator initialization\n");
          continue;
        }

      char *stanza = poeGenerateOneginStanza(&generator);

      if (stanza == NULL)
        printf("    error during stanza generation occured\n");
      printf("%s\n", stanza);
      free(stanza);

      continue;
    }

    if (strcmp(buffer, commandSort) == 0) {
      if (!textIsInit) {
        printf("    no text to write\n");
        continue;
      }

      if (generatorIsInit)
        poeDestroyGenerator(&generator);

      PoeStringCompareFn compareFn = NULL;

      if (strcmp(next, commandSortForward) == 0) {
        compareFn = poeCompareFromStart;
      } else if (strcmp(next, commandSortReverse) == 0) {
        compareFn = poeCompareFromEnd;
      } else if (strcmp(next, commandSortInitial) == 0) {
        compareFn = poeCompareInitialOrder;
      } else {
        printf("    unknown sorting method: \'%s\'\n", next);
        continue;
      }

      poeSortText(&text, compareFn);
      continue;
    }

    if (strcmp(buffer, commandWrite) == 0) {
      if (!textIsInit) {
        printf("    no text to write\n");
        continue;
      }

      fopen_s(&file, next, "w");

      if (file == NULL) {
        printf("    can't open \'%s\' file for text write\n", next);
        continue;
      }

      poeWriteText(file, &text);
      fclose(file);

      continue;
    }

    if (strcmp(buffer, commandQuit) == 0) {
      doContinue = POE_FALSE;
      continue;
    }

    printf("    unknown command: %s\n", buffer);
  }

  if (generatorIsInit)
    poeDestroyGenerator(&generator);
  if (textIsInit)
    poeDestroyText(&text);

  return 0;
} // main function end

// main.c file end