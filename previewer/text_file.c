#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <uchardet.h>
#include <iconv.h>

#define BUFFER_SIZE (1024*sizeof(char))

/**
 * get_charset():
 * get the file content charset by mozilla's universal chardet library
 *
 * fd: the file which content encoding need to be detect
 *
 * Return: an str contains enconding name if we can recoginze that
 */
const char *get_charset(FILE *fd, const int MAXLN)
{
  const char *charset;

  char *buffer = (char *)malloc(BUFFER_SIZE);
  if (buffer == NULL)
  {
    fputs("can't allocate memory", stderr);
    exit(1);
  }


  uchardet_t handle = uchardet_new();  
  for(int i = 0; i <= MAXLN && !feof(fd); i++)
  {
    size_t len = fread(buffer, sizeof(char), BUFFER_SIZE, fd);
    if( uchardet_handle_data(handle, buffer, len) != 0 )
    {
      fputs("Handle data error\n", stderr);
      exit(1);
    }
  }
  uchardet_data_end(handle);

  charset = uchardet_get_charset(handle);

  uchardet_delete(handle);
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
  printf("\033[1;34mencoding: %s\033[0m\n", (*charset) ? charset : "ascii");

  char *buffer = (char *)malloc(BUFFER_SIZE);
  if (buffer == NULL)
  {
    fputs("can't allocate memory", stderr);
    exit(1);
  }

  char *out_buffer  = (*charset) ? (char *)malloc(BUFFER_SIZE) : buffer;
  iconv_t converter = (*charset) ? iconv_open("utf8", charset) : NULL;
  
  for(int i = 0; i < maxln && fgets(buffer, BUFFER_SIZE, fd) != NULL; i++)
  {
    if (converter)
    {
      size_t in_bytes  = BUFFER_SIZE;
      size_t out_bytes = BUFFER_SIZE;
      
      char *in_buf  = buffer;
      char *out_buf = out_buffer;

      iconv(converter, &in_buf, &in_bytes, &out_buf, &out_bytes);
    }

    fputs(out_buffer, stdout);
  }

  if (*charset)
    free(out_buffer);
  free(buffer);

  return;
}


