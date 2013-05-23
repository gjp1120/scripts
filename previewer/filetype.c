#include <stdlib.h>
#include "scope.h"


const char *media_file[] = {"flac", "rmvb", "flv", "mkv", "ogg", "mp3", "mp4", "f4v", "avi", NULL};
const char *text_file[]  = {"txt", "log", "cue", "url", "lrc",NULL};
const char *webpage[]    = {"htm", "html", NULL};
const char *source_file[]= {"c", "cxx", "cpp", "c++",  "hpp", "cs", "h",   // C/C++/C# Source
                            "s", "asm",                                    // Asm Source
                            "pl", "pm", "t",                               // Perl
                            "py", "pyw", "pyx",                            // Python
                            "am", "m4",                                    // Makefile 
                            "sh",                                          // Shell
                            "java", "js",                                  // Java
                            NULL};                            
const char *archive[]    = {"Z", "bz2", "xz", "7z", "gz", NULL};


