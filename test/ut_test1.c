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

  dbgtst("Int by default") {
    x = val(0xABCD);
    dbgchk(valisint(x));
    printf("x=%04X\n",valtoint(x));
  }
 

  dbgtst("Int short") {
    int32_t l = 32;
    x = val(l);
    dbgchk(valisint(x));
    printf("x=%04X\n",valtoint(x));
  }

  dbgtst("pointer") {
    x = val((void *)"pippo");
    dbgchk(valisptr(x));
    printf("x=%s\n",(char *)valtoptr(x));
    
  }

  dbgtst("string") {
    x = val("pippo");
    dbgchk(valisstr(x));
    printf("x=%s\n",valtostr(x));
    valfree(x);
  }

  x = val(3.14);
  dbgchk(valisdbl(x));
  printf("x=%f\n",valtodbl(x));

  float f = 3.14;
  x = val(f);
  dbgchk(valisdbl(x));
  printf("x=%f\n",valtodbl(x));

  uint8_t c='A';
  x=val(c);
  dbgchk(valisint(x));
  printf("x=%02X\n",valtoint(x));

  dbgchk(valisbool(valtrue));
  dbgchk(valisbool(valfalse));

  dbgchk(valtoint(valtrue) == true);

  dbgchk(!valisdbl(valtrue));

}