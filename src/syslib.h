#ifndef SYSLIB_H
#define SYSLIB_H

#include <stdint.h>
#include "val.h"

typedef void (*syslib_f)(val_t);

syslib_f sys_cname2fptr(uint64_t cname);
syslib_f sys_fptr(int fndx);

#define kanji void
#endif