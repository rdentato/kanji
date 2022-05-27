
#include "val.h"
#include "dbg.h"

val_t io.print(val_t arg)
{
  dbgtrc("IO.PRINT: %lX",arg);
  switch(VALTYPE(arg)) {
    case VALINT: printf("%d ",valtoint(arg)); break;
    case VALDBL: printf("%f ",valtodbl(arg)); break;
    case VALNIL: printf("\n");
  }
  return valnil;
}


