#ifndef KANJI_SYS_H
#define KANJI_SYS_H
#include <stdint.h>
#include <ctype.h>

int64_t kaj_sys_encode(char **start);
char *kaj_sys_decode(int64_t fname);
#endif
