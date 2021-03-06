#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "scope.h"
#include "text_file.h"

/**
 * const char *get_ext():
 * get a str which contains file's ext 
 * WARN:
 *  this sub will end the process if can't fond any "ext"
 *  you can't disable this
 *
 * path : file's real path str
 *
 * Return: a str if seccess
 *         you should free() it
 */

char *get_ext(const char *path)
{
  char *dot_addr = strrchr(path, '.');

  if(!dot_addr)
    exit(RET_NO_PREVIEW);

  return strdup(++dot_addr);
}

/**
 * int in_array():
 * find a string in an string array (must end with NULL)
 *
 * str  : the target string that should found
 * array: an array that string may in it
 *
 * Return: 1 is found
 */
int in_array(const char *str, const char **array)
{
  while (*array != NULL)
  {
    if (strcasecmp(str, *array) == 0) return 1;
    array++;
  }

  return 0;
}

/**
 * main()
 * the program's entrance point
 */
int main(int argc, char **argv)
{
  if (argc != 4)
  {
    puts("Usage: scope <filename> <width> <height>");
    return RET_FAILURE;
  }

  const int maxln = atoi(argv[3]);
  char *ext = get_ext(argv[1]);
  const char *path = argv[1];

  if (in_array(ext, media_file))
  {
    free(ext);
    execlp("mediainfo", "mediainfo", path, NULL);
  }
  else if (in_array(ext, text_file))
  {
    free(ext);
    FILE *fd = NULL;

    if ( !(fd = fopen(path, "rb") ))
    {
      fputs("Can't open file\n", stderr);
      return RET_NO_PREVIEW;
    }

    print_plain_text_file(fd, maxln);

    fclose(fd);

    return RET_SUCCESS;
  }
  else if (in_array(ext, source_file))
  {
    free(ext);
    execlp("highlight", "highlight", "--out-format=ansi", path, NULL);
  }
  else if (in_array(ext, webpage))
  {
    free(ext);
    // Try w3m first
    execlp("w3m", "w3m", "-dump", path, NULL);
    execlp("lynx", "lynx", "-dump", path, NULL);
  }

  return RET_NO_PREVIEW;
}
