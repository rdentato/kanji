/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

#include <stdio.h>
#include <stdint.h>

#define SKP_MAIN
#include "skp.h"

#define DEBUG DEBUG_TEST
#define DBG_MAIN
#include "dbg.h"

#include "../fname.h"

#define MAXBUFSIZE 1024
char buf[MAXBUFSIZE];

int main(int argc, char *argv[])
{
  char *p = buf;
  uint64_t cname = 0;
  int len;
  int count = 0;

  while (fgets(buf,MAXBUFSIZE,stdin)) {
    p = buf;
    if (skp(p,"S 'kanji' S ",&p) ) {
      cname = encoded_fname(p);

      if (cname != INVALID_FNAME) {
        count++;
        len = encoded_fname_len(cname);
        fprintf(stdout,"{ 0x%016lX, %.*s },\n",cname, len, p);
        fprintf(stderr,"void %.*s(val_t);\n",len,p);
      }
    }
  }
  
  fprintf(stderr,"#define SYSLIB_COUNT %d\n",count);

  return 0;
}
