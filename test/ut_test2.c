// © 2021 Remo Dentato (rdentato@gmail.com)
// ⚖ MIT (https://opensource.org/licenses/MIT)


#define DEBUG DEBUG_TEST
#define DBG_MAIN
#include "dbg.h"

#define VAL_MAIN
#include "val.h"

int main(int argc, char *argv[])
{
  val_t x;

  dbgtst("xx") {
    x = val("pippo");
    dbgchk(valisbuf(x),"x: %lX",x);
    dbgchk(valsize(x) >= 6);
    dbgchk(valcount(x) == 6);
    valfree(x);
  }

  dbgtst("kk") {
    x = val((void *)main);
    dbgchk(valisptr(x));
    dbgchk(valtoptr(x) == (void *)main);
    
  }

}