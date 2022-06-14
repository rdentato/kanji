#include "dbg.h"
#include "knight_parse.h"
#include "val.h"

void die(char *err)
{
  fprintf(stderr,"FATAL ERROR: %s\n",err);
  exit(1);
}

int varcmp(void *, void *);

int32_t findvar(val_t vars, char *var)
{
  val_t varname = val(var);
  val_t *arr;
  int32_t i = 0;
  int32_t j = -1;
  int32_t m = -1;
  int cmp;

  j = valcount(vars) -1;
  arr = valarray(vars);
  while (i<=j) {
    m = (i+j)/2;
    cmp = varcmp(&varname,arr+m);
    if (cmp == 0) return m;
    if (cmp < 0) j = m-1;
    else i = m+1;
  }

  return -1;
}

void assignvar(val_t stack,val_t vars_value)
{
  val_t a,b;

  a = valtop(stack,-2); // value
  b = valtop(stack);    // variable

  if (!valisint(b)) die("Can't assign");

  valdrop(stack,1);

  valset(vars_value,b,a);
}

void output(FILE *f, val_t a)
{
  dbgtrc("OUT TYPE: %d",VALTYPE(a));

  switch(VALTYPE(a)) {
    case VALINT:  fprintf(f, "%d\n",valtoint(a)); break;
    case VALDBL:  fprintf(f, "%f\n",valtodbl(a)); break;
    case VALNIL:  fprintf(f, ".NIL.\n"); break;
    case VALSTR:  fprintf(f, "%s\n",(char*)valtoptr(a)); break;
    case VALBUF:  fprintf(f, "%s\n",(char*)valtostr(a)); break;
    default    :  if (a == valtrue) fprintf(f,".TRUE.\n"); 
                  else if (a == valfalse) fprintf(f,".TRUE.\n"); 
  }
}

void dofunc(val_t stack, char f)
{
  val_t a, b;
  int32_t i32_r;

  switch (f) {
    case '+' : a = valtop(stack,-2);
               b = valtop(stack);
               dbgtrc("ADD: %lX %lX",a,b);
               valdrop(stack,2);
               valpush(stack,valadd(a,b));
               break;

    case '-' : a = valtop(stack,-2);
               b = valtop(stack);
               dbgtrc("SUB: %lX %lX",a,b);
               valdrop(stack,2);
               valpush(stack,valsub(a,b));
               break;

    case '*' : a = valtop(stack,-2);
               b = valtop(stack);
               dbgtrc("MUL: %lX %lX",a,b);
               valdrop(stack,2);
               valpush(stack,valmul(a,b));
               break;

    case '/' : a = valtop(stack,-2);
               b = valtop(stack);
               valdrop(stack,2);
               dbgtrc("DIV: %lX %lX = %lX",a,b,val(valtoint(a) / valtoint(b)));
               valpush(stack, val(valtoint(a) / valtoint(b)));
               break;

    case '%' : a = valtop(stack,-2);
               b = valtop(stack);
               dbgtrc("MOD: %lX %lX",a,b);
               valdrop(stack,2);
               valpush(stack,valmod(a,b));
               break;

    case '~' : a = valtop(stack);
               dbgtrc("NEG: %lX",a);
               valdrop(stack);
               valpush(stack,valneg(a));
               break;

    case ';' : a = valtop(stack);
               valdrop(stack,2);
               valpush(stack,a);
               break;

    case 'O' : a = valtop(stack);
               output(stdout,a);
               break;

    case 'T' : valpush(stack,valtrue);  break;
    case 'N' : valpush(stack,valnil);   break;
    case 'F' : valpush(stack,valfalse); break;

    case '<' : a = valtop(stack,-2);
               b = valtop(stack);
               valdrop(stack,2);
               i32_r = valcmp(a,b);
               dbgtrc("LT: %lX %lX (%d)",a,b,i32_r);
               valpush(stack, (i32_r < 0)?valtrue:valfalse);
               break;

    case '>' : a = valtop(stack,-2);
               b = valtop(stack);
               valdrop(stack,2);
               i32_r = valcmp(a,b);
               dbgtrc("GT: %lX %lX (%d)",a,b,i32_r);
               valpush(stack, (i32_r > 0)?valtrue:valfalse);
               break;

    case '?' : a = valtop(stack,-2);
               b = valtop(stack);
               valdrop(stack,2);
               i32_r = valcmp(a,b);
               dbgtrc("EQ: %lX %lX (%d)",a,b,i32_r);
               valpush(stack,(i32_r == 0)?valtrue:valfalse);
               break;
  }
}

int isfalse(val_t a) 
{
  return valtoint(a) == 0;
}

val_t kneval(ast_t astcur)
{
  val_t stack = valvec(100);
  val_t vars = valnil;
  val_t vars_value = valnil;
  int32_t curnode = astroot(astcur);
  val_t top ;
  char *start;

    dbgtrc("4 ? 10: %d",valcmp(val(4),val(10)));

  if (astaux(astcur) == NULL) return -1;
  vars = *((val_t *)astaux(astcur));
  
  if (valisvec(vars)) {
    vars_value = valvec(valcount(vars));
    val_t *a = valarray(vars_value);
    if (a == NULL) die("Unexpected!");
    for (int k=0; k<valcount(vars); k++) 
      a[k] = valnil;
  }

  while (curnode != ASTNULL ) {
    if (astisnodeentry(astcur,curnode)) {
      start = astnodefrom(astcur,curnode);
     _dbgtrc("NODE: %d",curnode);
      if (astnodeis(astcur,curnode,number)) {
        dbgtrc("NUM: %d", atoi(start));
        valpush(stack,val(atoi(start)));
      }
      else if (astnodeis(astcur,curnode,variable)) {
        int v = findvar(vars,start);
        dbgtrc("VAR: %d (%.4s)", v,start);
        valpush(stack,valget(vars_value,val(v)));
      }
      else if (astnodeis(astcur,curnode,varref)) {
        int v = findvar(vars,start);
        dbgtrc("VARREF: %d (%.4s)", v,start);
        valpush(stack,val(v));
      }
      else if (astnodetag(astcur,curnode) == 255) {
        dbgtrc("FNC: %c ", *start);
        dofunc(stack, *start);
      }
      else if (astnodeis(astcur,curnode,assign)) {
        if (vars_value == valnil) die("No variable defined/referenced");
        dbgtrc("ASSIGN:");
        assignvar(stack,vars_value);
      }
      else if (astnodeis(astcur,curnode,while_check)) {
        val_t a = valtop(stack);
        valdrop(stack);
        if (isfalse(a)) curnode = astlast(astcur,curnode);
      }
      else if (astnodeis(astcur,curnode,while_end)) {
        curnode = astup(astcur,curnode);
      }
      else if (astnodeis(astcur,curnode,if_then)) {
        val_t a = valtop(stack);
        valdrop(stack);
        dbgtrc("IF: %lX",a);
        if (isfalse(a)) {
          curnode = astright(astcur,curnode);
          curnode = astright(astcur,curnode);
        }
      }
      else if (astnodeis(astcur,curnode,if_else)) {
          curnode = astright(astcur,curnode);
          curnode = astright(astcur,curnode);
      }

    }
    curnode = astnext(astcur,curnode);
  }
  top = valtop(stack);
  valfree(vars_value);
  valfree(stack);
  return top;
}