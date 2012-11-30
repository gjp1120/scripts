#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <uchardet.h>
#include <iconv.h>

#define BUFFER_SIZE (100*1024*sizeof(char))

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
 * fd:      the file that should print out
 * maxln:   as its name, tell this func how many lines should be print out
 * charset: can be fd's enconding or blank(if the file can be safety print out by 'ascii')
 *
 * print file content that already decoded to stdout
 *
 * FIXME:
 *  can't work
 */

void print_plain_text_file(FILE *fd, const int maxln, const char *charset)
{
  if (! *charset) charset = NULL;
  printf("\033[1;34mencoding: %s\033[0m\n", (charset) ? charset : "ascii");

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

  size_t avail  = BUFFER_SIZE - 1;
  size_t insize = 0;

  char *outptr = out_buffer;

  while ( avail > 0 )
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
            perror("...........");
            //exit(1);
            break;
          }
      }
  }
     
  /*   
  for(int i = 0; i < maxln && fgets(buffer, BUFFER_SIZE, fd) != NULL; i++)
  {
    if (converter)
    {
      size_t in_bytes  = BUFFER_SIZE;
      size_t out_bytes = BUFFER_SIZE;
      
      char *in_buf  = buffer;
      char *out_buf = out_buffer;

      iconv(converter, &in_buf, &in_bytes, &out_buf, &out_bytes);
      if (errno)
          puts(strerror(errno));
    }

    fputs(out_buffer, stdout);
  }*/

  fputs(out_buffer, stdout);

  if (charset)
    free(out_buffer);
  free(buffer);

  return;
}


