#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

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
 */

const char *get_ext(const char *path)
{
  char *ext = (char *)malloc(12*sizeof(char));
  char *dot_addr = strrchr(path, '.');

  if(!dot_addr)
    exit(RET_NO_PREVIEW);

  strncpy(ext, ++dot_addr, 12);

  return ext;
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
    if (strcmp(str, *array) == 0) return 1;
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
  const char *ext = get_ext(argv[1]);

  if (in_array(ext, media_file))
  {

  }
  else if (in_array(ext, text_file))
  {
    FILE *fd = NULL;

    if ( !(fd = fopen(argv[1], "rb") ))
    {
      fputs("Can't open file\n", stderr);
      return RET_NO_PREVIEW;
    }

    const char *charset = get_charset(fd, maxln);
    rewind(fd);
    print_plain_text_file(fd, maxln, charset);

    fclose(fd);

    return RET_SUCCESS;
  }

  return RET_NO_PREVIEW;
}
