//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#include <stdio.h>
#include <stdlib.h>
#include "kanji_sys.h"

#ifdef SYS_MAIN
#define SKP_MAIN
#define DBG_MAIN
#include "skp.h"
#endif

#include "dbg.h"

int64_t kaj_sys_encode(char **start)
{
  char *s = *start;
  int64_t fnum = 0;
  uint8_t c;

  if (!s || !*s) return 0;

  while (isspace((int)*s)) s++;

  for (int k=0; k<12; k++, s++) {
    c = (uint8_t)*s;
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
      c &= 0x1F;
    else if ((c == '.') || ('0' <= c && c <= '9'))
      c -= 18;
    else break;
   _dbgtrc("fnum: %lX c: %d -> %lX",fnum,c,(fnum * 40) | c);
    fnum = (fnum * 40) + c;
  }

  while (isspace((int)*s)) s++;

  *start = s;
  return fnum;
}

static char *kaj_sys_letter=" abcdefghijklmnopqrstuvwxyz . 0123456789";

char *kaj_sys_decode(int64_t fname)
{
  static char s[16];
  int cnt = 15;
  s[cnt--] = '\0';
  while (fname != 0 && cnt>=0) {
    s[cnt--] = kaj_sys_letter[fname % 40];
    fname /= 40;
  }
  return s+cnt+1;
}

#ifdef SYS_MAIN

#define BUFLEN 1024
char buf[BUFLEN];

int main(int argc, char *argv[])
{
  char *s;
  char *p;
  char  c;
  while ((s = fgets(buf,BUFLEN,stdin))) {
    p=s;
    fprintf(stdout,"{ 0x%016lX, sys_",kaj_sys_encode(&p));
    for (p=s; !isspace((int)*p); p++) {
      c = (*p == '.')? '_' : *p;
      fputc(c, stdout);
    }
    fputs(" },\n",stdout);
  }
}

#endif 