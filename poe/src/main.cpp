/**
 * @file   main.cpp
 * @author tiot2
 * @brief  project main module
 */

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
cliGetString(
  char *const buffer,
  const size_t bufferSize
) {
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
  // little setup
  srand((unsigned int)time(NULL));
  system("chcp 1251");
  system("cls");

  PoeText text = {0};
  PoeBool textIsInit = POE_FALSE;
  PoeOneginGenerator generator = {0};
  PoeBool generatorIsInit = POE_FALSE;
  FILE *file = NULL;

  static const struct {
    const char
      *load       ,
      *stanza     ,
      *help       ,
      *sort       ,
      *sortInitial,
      *sortForward,
      *sortReverse,
      *write      ,
      *quit       ;
  } command = {
    .load        = "load",
    .stanza      = "stanza",
    .help        = "help",
    .sort        = "sort",
    .sortInitial = "initial",
    .sortForward = "forward",
    .sortReverse = "reverse",
    .write       = "write",
    .quit        = "quit",
  };

  char buffer[512] = {0};

  PoeBool doContinue = POE_TRUE;

  // ERROR CODES
  // CLI

  while (doContinue) {
    printf(">>> ");

    cliGetString(buffer, sizeof(buffer));

    // check if command should be executed in cmd
    if (buffer[0] == '!') {
      system(buffer + 1);
      continue;
    }

    const char *commandData = "";

    // split command and next part
    {
      char *n = strchr(buffer, ' ');

      if (n != NULL) {
        *n = '\0';
        commandData = n + 1;
      }
    }

    if (strcmp(buffer, command.load) == 0) {
      if (generatorIsInit) {
        // generator deinitialization
        poeDestroyOneginGenerator(&generator);
        generatorIsInit = POE_FALSE;

        // text initialization
        poeDestroyText(&text);
        textIsInit = POE_FALSE;
      }

      // TODO Handle errorcode
      int errCode = fopen_s(&file, commandData, "rb");


      if (file == NULL) {
        char errBuffer[512] = {0};

        if (strerror_s(errBuffer, sizeof(errBuffer) - 1, errCode) != 0)
          strcpy_s(errBuffer, sizeof(errBuffer) - 1, "unknown");

        printf("    error during \'%s\' file open: %s\n", commandData, errBuffer);
        continue;
      }

      if (POE_CHECK(poeParseText(file, &text)))
        textIsInit = POE_TRUE;
      else
        printf("    error during text file parsing occured\n");

      fclose(file);

      continue;
    }

    if (strcmp(buffer, command.help) == 0) {
      printf("    load file              %s <file name>\n"           , command.load);
      printf("    generate stanza        %s\n"                       , command.stanza);
      printf("    sort with comparator   %s <\'%s\'|\'%s\'|\'%s\'>\n", command.sort, command.sortInitial, command.sortForward, command.sortReverse);
      printf("    write to file          %s <file name>\n"           , command.write);
      printf("\n");
      printf("    show this menu         %s\n"                       , command.help);
      printf("    quit from program      %s\n"                       , command.quit);
      printf("    execute cmd command    !<command>\n");
      continue;
    }

    if (strcmp(buffer, command.stanza) == 0) {
      if (!textIsInit) {
        printf("    no text to generate stanza from\n");
        continue;
      }

      if (!generatorIsInit)
        if (POE_CHECK(poeCreateOneginGenerator(&text, &generator))) {
          generatorIsInit = POE_TRUE;
        } else {
          printf("    error during text generator initialization\n");
          continue;
        }

      const PoeString *stanzaBuffer[14] = {NULL};
      if (!POE_CHECK(poeOneginGenerateStanza(&generator, stanzaBuffer))) {
        printf("    error during stanza generation occured\n");
        continue;
      }

      for (size_t i = 0; i < 14; i++)
        printf("%s\n", stanzaBuffer[i]->begin);
      continue;
    }

    if (strcmp(buffer, command.sort) == 0) {
      if (!textIsInit) {
        printf("    no text to write\n");
        continue;
      }

      if (generatorIsInit) {
        poeDestroyOneginGenerator(&generator);
        generatorIsInit = POE_FALSE;
      }

      PoeStringCompareFn compareFn = NULL;

      if (strcmp(commandData, command.sortForward) == 0) {
        compareFn = poeCompareFromStart;
      } else if (strcmp(commandData, command.sortReverse) == 0) {
        compareFn = poeCompareFromEnd;
      } else if (strcmp(commandData, command.sortInitial) == 0) {
        compareFn = poeCompareInitialOrder;
      } else {
        printf("    unknown sorting method: \'%s\'\n", commandData);
        continue;
      }

      poeSortText(&text, compareFn);
      continue;
    }

    if (strcmp(buffer, command.write) == 0) {
      if (!textIsInit) {
        printf("    no text to write\n");
        continue;
      }

      fopen_s(&file, commandData, "w");

      if (file == NULL) {
        printf("    can't open \'%s\' file for text write\n", commandData);
        continue;
      }

      poeWriteText(file, &text);
      fclose(file);

      continue;
    }

    if (strcmp(buffer, command.quit) == 0) {
      doContinue = POE_FALSE;
      continue;
    }

    printf("    unknown command: %s\n", buffer);
  }

  if (generatorIsInit)
    poeDestroyOneginGenerator(&generator);
  if (textIsInit)
    poeDestroyText(&text);

  return 0;
} // main function end

// main.c file end
