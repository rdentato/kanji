
#include "val.h"
#include "dbg.h"

/*
  io.printf
  io.open
  io.close
  io.pos
  io.seek
  io.stderr
  io.stdout
  io.stdin
  io.read
  io.write
*/

val_t io.print(val_t args)
{
 _dbgtrc("IO.PRINT: %lX %04X",arg,VALTYPE(arg));
  FILE *f = stdout;
  val_t arg = args;
  if (valisvec(args) && valcount(args)>1) {
    arg = valget(args,val(0));
    if (valisptr(arg)) {
      f = valtoptr(arg);
      arg = valget(args,val(1));
    }
    else return valnil;
  }
  switch(VALTYPE(arg)) {
    case VALINT: fprintf(f,"%d ",valtoint(arg)); break;
    case VALDBL: fprintf(f,"%f ",valtodbl(arg)); break;
    case VALNIL: fprintf(f,"(nil)"); break;
    case VALSTR: fprintf(f,"%s",(char*)valtoptr(arg)); break;
    case VALBUF: fprintf(f,"%s",(char*)valtostr(arg)); break;
  }
  return valnil;
}

// name, r/w
val_t io.open(val_t args)
{
  char *name = NULL;
  char *mode = NULL;
  if (valisvec(args)) {
    name = valtostr(valget(args,val(0)));
    mode = valtostr(valget(args,val(1)));
  }
  else name = valtostr(args);

  if (name == NULL) return valnil;
  if (mode == NULL) mode = "rb";
  dbgtrc("io.open('%s','%s')",name,mode);
  FILE *f = fopen(name,mode);
  if (f == NULL) return valnil;
  return (val((void *)f));
}

val_t io.close(val_t file)
{
  if (valisptr(file)) {
    dbgtrc("io.close(%p)",valtoptr(file));
    fclose(valtoptr(file));
  }
  return valnil;
}

