
#include "val.h"
#include "dbg.h"

val_t io.print(val_t arg)
{
 _dbgtrc("IO.PRINT: %lX %04X",arg,VALTYPE(arg));
  switch(VALTYPE(arg)) {
    case VALINT: printf("%d ",valtoint(arg)); break;
    case VALDBL: printf("%f ",valtodbl(arg)); break;
    case VALNIL: printf("\n"); break;
    case VALSTR: printf("%s",(char*)valtoptr(arg)); break;
  }
  return valnil;
}


