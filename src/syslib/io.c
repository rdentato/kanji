
#include <stdio.h>
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

static int prt(FILE *f,val_t arg)
{
  switch(VALTYPE(arg)) {
    case VALINT: return fprintf(f,"%d ",valtoint(arg));
    case VALDBL: return fprintf(f,"%f ",valtodbl(arg));
    case VALNIL: return fprintf(f,"(nil)");
    case VALSTR: return fprintf(f,"%s",(char*)valtoptr(arg));
    case VALBUF: return fprintf(f,"%s",(char*)valtostr(arg));
  }
  return 0;
}

val_t io.print(val_t args)
{
 _dbgtrc("IO.PRINT: %lX %04X",arg,VALTYPE(arg));
  FILE *f = stdout;
  val_t arg = args;
  int k = 0;
  if (valisvec(args) && valcount(args)>1) {
    arg = valget(args,val(k));
    if (valisptr(arg)) {
      f = valtoptr(arg);
      k++;
      arg = valget(args,val(k));
    }
    do {
      prt(f,arg); k++;
      arg = valget(args,val(k));
    } while (k<valcount(args));
  }
  else prt(f,arg);
  
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

val_t io.stderr(val_t arg)
{  return val((void *)stderr); }

val_t io.stdout(val_t arg)
{  return val((void *)stdout); }

val_t io.stdin(val_t arg)
{  return val((void *)stdin); }
