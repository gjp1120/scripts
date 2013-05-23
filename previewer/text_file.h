#ifndef __TEXT_FILE_H
#define __TEXT_FILE_H

const char *get_charset_r(FILE *fd, const size_t MAXLN,
                          const size_t buffer_size, char *buffer);
void print_plain_text_file(FILE *fd, const int maxln);

#endif //__TEXT_FILE_H
