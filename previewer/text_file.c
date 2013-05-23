#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <uchardet.h>
#include <iconv.h>

#include "scope.h"

#define BUFFER_SIZE (100*1024*sizeof(char))

/**
 * get_charset_r():
 * get the file content charset by mozilla's universal chardet library
 *
 * fd: the file which content encoding need to be detect
 *
 * Return: an str contains enconding name if we can recoginze that
 */
const char *get_charset_r(FILE *fd, const size_t MAXLN,
                          const size_t buffer_size, char *buffer)
{
  const char *charset;

  uchardet_t handle = uchardet_new();
  for(int i = 0; i <= MAXLN && !feof(fd); i++)
  {
    size_t len = fread(buffer, sizeof(char), buffer_size, fd);
    if( uchardet_handle_data(handle, buffer, len) != 0 )
    {
      fputs("Handle data error\n", stderr);
      exit(1);
    }
  }
  uchardet_data_end(handle);

  charset = strdup(uchardet_get_charset(handle));

  uchardet_delete(handle);
  return charset;
}

/**
 * print_plain_text_file():
 * 
 * fd:      the file that should print out
 * maxln:   as its name, tell this func how many lines should be print out
 * charset: can be fd's enconding or blank(if the file can be safety print out by 'ascii')
 *
 * print file content that already decoded to stdout
 *
 * FIXME
 *  recode
 */

#if 0
void print_plain_text_file(FILE *fd, const int maxln, const char *charset)
{
  if (! *charset) charset = NULL;
  
  printf("\033[1;34mencoding: %s\033[0m\n", (charset) ? charset : "ascii");
  fflush(stdout);

  char *buffer = (char *)malloc(BUFFER_SIZE);
  if (buffer == NULL)
  {
    fputs("can't allocate memory", stderr);
    exit(1);
  }

  char *encoding;
  if (charset)
  {
    encoding = (char *)malloc(20);
    strcpy(encoding, charset);
    strcat(encoding, "//IGNORE");
  }
  //else encoding = "ascii";

  char *out_buffer  = (charset) ? (char *)malloc(BUFFER_SIZE) : buffer;
  iconv_t converter = (charset) ? iconv_open("utf8", encoding) : NULL;
  if (converter == (iconv_t)-1)
  {
    if (errno == EINVAL)
    {
      fprintf(stderr, "conversion from '%s' to 'utf8' not available", charset);
      exit(1);
    }
    else
    {
      fputs("unknow error happends in 'iconv()'\n", stderr);
      exit(1);
    }
  }
  
  size_t avail    = (charset) ? (BUFFER_SIZE - 1) : 0; //if charset is ascii,we shouldn't read anything.
  size_t insize   = 0;
  size_t out_size = 0;

  char *outptr    = out_buffer;

  while ( avail > 8 )
  {     
    /* Read more input.  */
    size_t nread = read (fileno(fd), buffer + insize, BUFFER_SIZE - insize);
    if (nread == 0)
      {
        /* Now write out the byte sequence to get into the
         * initial state if this is necessary.  */
        iconv (converter, NULL, NULL, &outptr, &avail);
        break;
      }
    insize += nread;
     
    char *inptr  = buffer;

    /* Do the conversion.  */
    size_t nconv = iconv (converter, &inptr, &insize, &outptr, &avail);
    if (nconv == (size_t) -1)
      {
        /* Not everything went right.  It might only be
         * an unfinished byte sequence at the end of the
         * buffer.  Or it is a real problem.  */
        if (errno == EINVAL)
          /* This is harmless.  Simply move the unused
           * bytes to the beginning of the buffer so that
           * they can be used in the next round.  */
          memmove (buffer, inptr, insize);
        else if (errno == E2BIG) continue;
        else
          {
            /* It is a real problem.  Maybe we ran out of
             * space in the output buffer or we have invalid
             * input.  In any case back the file pointer to
             * the position of the last processed byte.  */
            perror("an unhandled error");
            //exit(1);
            break;
          }
      }
  }
  
  if (!charset)
    out_size += read(fileno(fd), buffer, BUFFER_SIZE);
  else
    out_size = outptr - out_buffer;

  //fputs(out_buffer, stdout);
  write(1, out_buffer, out_size);

  if (charset)
  {
    free(out_buffer);
    free(encoding);
  }
  free(buffer);

  if (converter) iconv_close(converter);

  return;
}
#endif //0

static char *_alloc_buffer()
{
  char *buffer = (char *)malloc(BUFFER_SIZE);
  if (buffer == NULL)
  {
    fputs("can't allocate memory", stderr);
    exit(RET_FAILURE);
  }

  return buffer;
}

static size_t _read_utf8_file(FILE *fd, size_t sz, char *buffer)
{
  size_t out = read(fileno(fd), buffer, sz);
  if (sz < 0)
  {
    perror("read");
    exit(RET_FAILURE);
  }
  return out;
}

static size_t _read_other_file(FILE *fd, size_t sz, char *buffer, const char *charset)
{
  //prepare encoding var
  char *encoding;
  if (charset)
  {
    encoding = (char *)malloc(20);
    strcpy(encoding, charset);
    strcat(encoding, "//IGNORE");
  }

  char *in_buffer = _alloc_buffer();

  // init iconv handle
  iconv_t converter = iconv_open("utf8", encoding);
  if (converter == (iconv_t)-1)
  {
    if (errno == EINVAL)
    {
      fprintf(stderr, "conversion from '%s' to 'utf8' not available", charset);
      exit(RET_FAILURE);
    }
    else
    {
      fputs("unknow error happends in 'iconv()'\n", stderr);
      exit(RET_FAILURE);
    }
  }
 
  // iconv loop(read, iconv) 
  size_t avail    = BUFFER_SIZE - 1;
  size_t insize   = 0;
  char  *outptr   = buffer;

  while ( avail > 8 )
  {     
    /* Read more input.  */
    size_t nread = read (fileno(fd), in_buffer + insize, BUFFER_SIZE - insize);
    if (nread == 0)
    {
      /* Now write out the byte sequence to get into the
       * initial state if this is necessary.  */
      iconv (converter, NULL, NULL, &outptr, &avail);
      break;
    }
    insize += nread;
     
    char *inptr  = in_buffer;

    /* Do the conversion.  */
    size_t nconv = iconv (converter, &inptr, &insize, &outptr, &avail);
    if (nconv == (size_t) -1)
    {
      /* Not everything went right.  It might only be
       * an unfinished byte sequence at the end of the
       * buffer.  Or it is a real problem.  */
      if (errno == EINVAL)
        /* This is harmless.  Simply move the unused
         * bytes to the beginning of the buffer so that
         * they can be used in the next round.  */
        memmove (buffer, inptr, insize);
      else if (errno == E2BIG) continue;
      else
      {
        /* It is a real problem.  Maybe we ran out of
         * space in the output buffer or we have invalid
         * input.  In any case back the file pointer to
         * the position of the last processed byte.  */
        perror("an unhandled error");
        //exit(1);
        break;
      }
    }
  }

  // free handle
  if (converter) iconv_close(converter);
  free(encoding);
  free(in_buffer);
  
  // calc iconv out_size (for write())
  return outptr - buffer;
}

void print_plain_text_file(FILE *fd, const int maxln)
{
  size_t out_size   = 0;
  char  *buffer     = _alloc_buffer();

  const char *charset = get_charset_r(fd, maxln, BUFFER_SIZE, buffer);
  rewind(fd);

  printf("\033[1;34mencoding: %s\033[0m\n", (*charset) ? charset : "ascii");
  fflush(stdout);

  if (*charset == 0 || !strcasecmp(charset, "utf-8"))
  {
    out_size = _read_utf8_file(fd, BUFFER_SIZE, buffer);
  }
  else
  {
    out_size = _read_other_file(fd, BUFFER_SIZE, buffer, charset);
  }
    
  //fputs(buffer, stdout);
  write(1, buffer, out_size);

  free(buffer);
  free((void *)charset);

  return;
}
