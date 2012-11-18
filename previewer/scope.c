#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uchardet.h>
#include <iconv.h>

#define BUFFER_SIZE 1024
#define MAXLN 32

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
    if (strcmp(str, *array)) return 1;
    array++;
  }

  return 0;
}

/**
 * get_charset():
 * get the file content charset by mozilla's universal chardet library
 *
 * fd: the file which content encoding need to be detect
 *
 * Return: an str contains enconding name if we can recoginze that
 */
const char *get_charset(FILE *fd)
{
  const char *charset;

  char *buffer = (char *)malloc(BUFFER_SIZE);

  uchardet_t handle = uchardet_new();  
  for(int i = 0; i <= MAXLN && !feof(fd); i++)
  {
    size_t len = fread(buffer, 1, BUFFER_SIZE, fd);
    if( uchardet_handle_data(handle, buffer, len) != 0)
    {
      fputs("Handle data error\n", stderr);
      uchardet_delete(handle);
      exit(-1);
    }
  }
  uchardet_data_end(handle);

  charset = uchardet_get_charset(handle);
  
  fclose(fd);
  free(buffer);

  return charset;
}

/**
 * print_plain_text_file():
 * 
 * FIXME:
 *  should use iconv
 */

void print_plain_text_file(FILE *fd, const int maxln, const char *charset)
{
  char *buffer = (char *)malloc(BUFFER_SIZE);
  char *out_buffer = charset ? (char *)malloc(BUFFER_SIZE) : NULL;
  iconv_t converter = charset ? iconv_open("utf8", charset) : NULL;
  
  for(int i = 0; i < maxln && fgets(buffer, BUFFER_SIZE, fd) != NULL; i++)
  {
    //if (converter) iconv(converter, &buffer, )
    fputs(buffer, stdout);
  }

  return;
}

/**
 * main()
 * the program's entrance point
 *
 */
int main(int argc, char **argv)
{
  if (argc > 2 || argc == 1)
  {
    puts("Usage: ucat <filename>");
    exit(-1);
  }

  FILE *fd = NULL;

  if ( !(fd = fopen(argv[1],"r") ))
  {
    fputs("Can't open file\n", stderr);
    exit(-1);
  }

  return 0;
}
