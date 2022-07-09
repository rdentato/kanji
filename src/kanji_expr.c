// © 2022 Remo Dentato (rdentato@gmail.com)
// ⚖ MIT (https://opensource.org/licenses/MIT)

#include "kanji.h"
#include "kanji_sys.h"

/*
  + - * / % & ! | ^ ~ < <= > >= = != 
  
  mth.sin

  d(up) s(wap) r(ot) z(ap) ? [ ]

  %0

  -1 %0 * 3 < ? [2 1 + ][ 5 2 *] ;

*/


val_t xpr_eval(kaj_pgm_t pgm,char *xpr)
{
  val_t stk;
  val_t ret;
  int32_t n;
  char *s = xpr;
  char *t;
  stk = valvec(30);
  dbgtrc("EVALUATING \"%s\"",xpr);

  while (*s) {
    switch (*s++) {
      case '+' : if (valcount(stk) < 2) break;
                 ret = valadd(valtop(stk,-1),valtop(stk,-2));
                 valdrop(stk,2);
                 valpush(stk,ret);
                 break;

      case '-' : if (!isdigit((int)s[1])) {
                   if (valcount(stk) < 2) break;
                   ret = valsub(valtop(stk,-1),valtop(stk,-2));
                   valdrop(stk,2);
                   valpush(stk,ret);
                   break;
                 }

                 break;

      case '*' : if (valcount(stk) < 2) break;
                 ret = valmul(valtop(stk,-1),valtop(stk,-2));
                 valdrop(stk,2);
                 valpush(stk,ret);
                 break;

      case '/' : if (valcount(stk) < 2) break;
                 ret = valdiv(valtop(stk,-1),valtop(stk,-2));
                 valdrop(stk,2);
                 valpush(stk,ret);
                 break;

      case '%' : if (!isxdigit(*s)) { // mod
                   if (valcount(stk) < 2) break; 
                   ret = valmod(valtop(stk,-1),valtop(stk,-2));
                   valdrop(stk,2);
                   valpush(stk,ret);
                   break;
                 }

                 // register
                 n = strtol(s+1,&t,16);
                 if ((n != REG_NONE) && (n<pgm->max_regs))
                   valpush(stk,pgm->lst.regs[n]);
                 s = t;
                 break;

    }
  }
  ret = valtop(stk);
  valfree(stk);
  return val(ret);
}

