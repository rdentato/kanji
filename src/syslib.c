/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

#include "syslib.h"
#include "fname.h"

#include "syslib/syslib_proto.h"

typedef struct {
  uint64_t cname;
  syslib_f fptr;
} syslib_t;

static syslib_t systab[] = {

   #include "syslib/syslib_tab.h"

};

static void syslib_f_NULL(val_t stack)
{  return; }

syslib_f sys_cname2fptr(uint64_t cname)
{
   return syslib_f_NULL;
}

syslib_f sys_fptr(int fndx)
{
   return syslib_f_NULL;
}

