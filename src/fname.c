/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

/*
  Function names are ancoded as 64bit numbers.
  To fit into such small space, the names have the following restrictions:
     - max 12 characters plus one dot (the initial one is discarded).
     - use only lowercase letters and numbers from 0 to 5 

  the dot is used to separate namespaces:

  .io.print
  .io.readln
  .....
  .mth.abs
  .mth.floor

*/

#include "fname.h"
#include "dbg.h"

static uint8_t chrcode(char c) 
{
  uint8_t code = ((c-'0') - 11 * (c>>6)) & 0x1F ;
  
 _dbgtrc("chr: '%c' code: %02X",c,code);
  return code;
}

int decode_fname(uint64_t cname, char *dest)
{
  uint64_t dotpos=0;
  int k=0;

  dotpos = 0xF000000000000000 >> 60;

  cname &= 0x0FFFFFFFFFFFFFFF; // clean dot position

  k=0;
  dest[k++] = '.';

  while (cname != 0) {
    if (k == dotpos) dest[k] = '.';
    else {
      dest[k] = "012345abcdefghijklmnopqrstuvwxyz"[cname & 0x1F];
      cname >>= 5;
    }
    k++;
  }

  return 0;
}

int encoded_fname_len(uint64_t cname)
{
  int len = 0;

  if (cname != 0) {
    cname &= 0x0FFFFFFFFFFFFFFF; // clean dot position
    len = 1;
    while ((cname != 0) && (len < 12)) {
      len++;
      cname >>= 5;
    }
  }
  return len;
}

uint64_t encoded_fname(char *fname)
{
  int dotpos = 0;
  uint64_t cname = 0;
  int len = 0;
  char *t = fname;

  while (1) {
    if (('0' <= *t && *t <= '5') ||
        ('a' <= *t && *t <= 'z')) {
      cname = (cname << 5) | chrcode(*t);
      len++;
      if (len == 12) break;
    } 
    else if (*t == '_' || *t == '.') {
      if (dotpos > 0) break;
      dotpos = len;
    }
    else break;
    t++;
  };

  if (len < 1 || 12 < len ) return INVALID_FNAME;
  
  return cname | ((uint64_t)dotpos) << 60;
}
