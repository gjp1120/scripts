#ifndef __TEXT_FILE_H
#define __TEXT_FILE_H

const char *get_charset(FILE *fd, const int MAXLN);
void print_plain_text_file(FILE *fd, const int maxln, const char *charset);

#endif //__TEXT_FILE_H
