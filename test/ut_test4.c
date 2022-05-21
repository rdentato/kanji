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
    valenq(x,val(2));
    dbgchk(valcount(x) == 1);

    valenq(x,val(20));
    dbgchk(valcount(x) == 2);

    val_t n;
    n = valhead(x);
    dbgchk(valtoint(n) == 2, "%lx",n);
    
    n = valtail(x);
    dbgchk(valtoint(n) == 20);
    
    int32_t t;
    t = valdeq(x);
    dbgchk(t == 1);
    dbgchk(valcount(x) == 1);

    n = valhead(x);
    dbgchk(valtoint(n) == 20, "%lx",n);
    
    n = valtail(x);
    dbgchk(valtoint(n) == 20);

    

    valfree(x);
  }
  
 
}