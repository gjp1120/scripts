#ifndef __SCOPE_H
#define __SCOPE_H

#include "filetype.h"

// Meanings of exit codes:
// code | meaning    | action of ranger
// -----+------------+-------------------------------------------
// 0    | success    | success. display stdout as preview
// 1    | no preview | failure. display no preview at all
// 2    | plain text | display the plain content of the file
// 3    | fix width  | success. Don't reload when width changes
// 4    | fix height | success. Don't reload when height changes
// 5    | fix both   | success. Don't ever reload
#define RET_SUCCESS    0
#define RET_FAILURE    1   //if previewer is failure, that mean we can't preview it
#define RET_NO_PREVIEW 1
#define RET_PLAIN_TEXT 2
#define RET_FIX_WIDTH  3
#define RET_FIX_HEIGHT 4
#define RET_FIX_BOTH   5

#endif //__SCOPE_H
