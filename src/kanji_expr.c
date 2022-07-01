// © 2022 Remo Dentato (rdentato@gmail.com)
// ⚖ MIT (https://opensource.org/licenses/MIT)

#include "kanji.h"
#include "kanji_sys.h"

/*
  + - * / % mth.sin & ! | ^ ~ < <= > >= = != d(up) s(wap) r(ot) z(ap)

  %0 [x]  
*/
val_t xpr_eval(kaj_pgm_t pgm,char *xpr)
{
  val_t stk;
  val_t op_a, op_b, ret;
  char *s = xpr;
  stk = valvec(30);
  dbgtrc("EVALUATING \"%s\"",xpr);

  while (*s) {
    switch (*s++) {
      case '+' : if (valcount(stk) < 2) break;
                 ret = valadd(valtop(stk,-1),valtop(stk,-2));
                 valdrop(stk,2);
                 valpush(stk,ret);
                 break;

      case '%' : if (!isxdigit(*s)) { // mod
                   break;
                 }

                 // register

                 break;

    }
  }

  valfree(stk);
  return val(0);
}

