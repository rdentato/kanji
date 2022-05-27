// © 2021 Remo Dentato (rdentato@gmail.com)
// ⚖ MIT (https://opensource.org/licenses/MIT)


#define DEBUG DEBUG_TEST
#define DBG_MAIN
#include "dbg.h"

#define VAL_MAIN
#include "val.h"

#include "inttypes.h"

int main(int argc, char *argv[])
{
  val_t x;

  dbgtst("Allocate vec") {
    x = valvec(100);
    dbgchk(valcount(x) == 0);
    dbgchk(valsize(x) >= 100);
    valfree(x);
  }
  
  dbgtst("Set to a not vec") {
    dbgchk(valset(val(0),val(1),val(3.4)) == valnil);
  }

  dbgtst("Set/get to a vec") {
    val_t ret;
    val_t v;

    v = valvec(100);
    ret = valset(v,val(1),val(2.3));
    dbgchk(ret > 0);
    x = valget(v,val(1));
    dbgchk(valisdbl(x));
    dbginf("v[1] = %g",valtodbl(x));
    ret = valset(v,val(230),val(23));
    dbgchk(valtoint(ret) > 0);
    dbgchk(valsize(v) >= 230);
    x = valget(v,val(230));
    dbgchk(valisint(x));

    valfree(v);
  }

  dbgtst("Stack 1") {
    int32_t n;
    val_t stack;
    val_t x;
    stack = valvec(100);
 
    n = valtoint(valpush(stack,val(2)));
    dbgchk(n==1,"n: %X (%d)",n,valtoint(n));
    x = valtop(stack);
    dbgchk(valisint(x),"x=0x%lx",x);
    dbgchk(valtoint(x) == 2,"%lX",x);
    valfree(stack);
  }

  dbgtst("Stack 2") {
    int32_t n;
    val_t stack;
    val_t x;
    stack = valvec(100);
    for (int k=0; k<10; k++)  {
      n = valpush(stack,val(100+k));
      dbgchk(n == k+1);
    }
    dbgchk(valcount(stack) == 10);
    valdrop(stack,3);
    dbgchk(valcount(stack) == 7);
    x = valtop(stack);
    dbgchk( (x != valnil) && (valtoint(x) == 106),"val: %d",valtoint(x));

    x = valtop(stack,-2);
    dbgchk( (x != valnil) && (valtoint(x) == 105),"val: %d",valtoint(x));
    
    x = valtop(stack,-200);
    dbgchk( (x == valnil) ,"val: %d",valtoint(x));
       
    valfree(stack);
  }
}