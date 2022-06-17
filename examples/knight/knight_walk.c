#include "dbg.h"
#include "knight_parse.h"
#include "val.h"
#include <stdlib.h>
#include <time.h>

static char *N_string="null";
static char *F_string="false";
static char *T_string="true";

void die(char *err)
{
  fprintf(stderr,"FATAL ERROR: %s\n",err);
  exit(1);
}

typedef struct eval_env_s {
  val_t stack;
  val_t vars;
  val_t vars_val;
  val_t bufs;
  val_t bufs_free;
} eval_env_t;

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

void assignvar(eval_env_t *env)
{
  val_t a,b;

  a = valtop(env->stack,-2); // value
  b = valtop(env->stack);    // variable

  if (!valisint(b)) die("Can't assign");

  valdrop(env->stack,1);

  valset(env->vars_val,b,a);
}

void output(FILE *f, val_t a)
{
  dbgtrc("OUT TYPE: %d",VALTYPE(a));
  int32_t n;
  int32_t nl = 1;
  char *p;

  switch(VALTYPE(a)) {
    case VALINT:  fprintf(f, "%d",valtoint(a)); break;
    case VALDBL:  fprintf(f, "%f",valtodbl(a)); break;
    case VALNIL:  fputs(N_string,f); break;

    case VALSTR:  
    case VALBUF:  n = valcount(a);
                  p = valtostr(a);
                  if (n>0 && p[n-1] == '\\') { n--; nl = 0;}
                  fprintf(f, "%.*s",n,p); break;

    default    :  if (a == valtrue) fputs(T_string,f);
                  else if (a == valfalse) fputs(F_string,f);; 
  }
  if (nl) fputc('\n',f);
}

val_t addbuf(eval_env_t *env)
{
  val_t buf;
 _dbgtrc("ADDING BUF");
  if (env->bufs_free != valnil) {
    buf = env->bufs_free;
   _dbgtrc("RECYCLING %lX",buf);
    env->bufs_free = valaux(buf);
    valaux(buf,valnil);
  }
  else {
    buf = valbuf(20);
   _dbgtrc("NEWBUF %lX",buf);
  }

  valaux(buf,env->bufs);
  env->bufs = buf;
  return buf;
}

int isfalse(val_t a) { return valtoint(a) == 0; }
int isstring(val_t a) { return (valisstr(a) || valisbuf(a));}

void dofunc_0(eval_env_t *env, char f)
{
  switch (f) {
    case 'T' : valpush(env->stack,valtrue);  break;
    case 'N' : valpush(env->stack,valnil);   break;
    case 'F' : valpush(env->stack,valfalse); break;
    case 'R' : valpush(env->stack,val(rand() & 0x7FFF)); break;
    case 'P' : { val_t prompt = addbuf(env);
                 int k = valbufgets(prompt,stdin);
                 char *arr=valtostr(prompt);
                 while (k>0 && (arr[k-1] == '\n' || arr[k-1] == '\r')) {
                   arr[--k] = '\0'; 
                   valcount(prompt,k);
                 }
                _dbgtrc("PROMPT: k=%d",k);
                 valpush(env->stack,prompt);
               }
  }
}

void dofunc_1(eval_env_t *env, char f)
{
  val_t a;
  int32_t n;

  a = valtop(env->stack);
               
  switch (f) {
    case 'Q' : exit(valtoint(a));

    case '~' : valdrop(env->stack);
               n = -valtoint(a);
               valpush(env->stack,val(n));
               break;

    case '!' : valdrop(env->stack);
               n = valtoint(a);
               dbgtrc("!:%d",n);
               valpush(env->stack,n ? valfalse : valtrue);
               break;

    case 'L' : valdrop(env->stack);
               n = valcount(a)-1;
               if (n<0) n = 0;
               valpush(env->stack,val(n));
               break;

    case 'A' : valdrop(env->stack);
               if (valisint(a)) {
                 n = valtoint(a);
                 val_t buf = addbuf(env);
                 valset(buf,val(0),val(n));
                 valset(buf,val(1),val(0));
                 valpush(env->stack,buf);
               }
               else if (isstring(a)) {
                 valpush(env->stack, val((int)(*valtostr(a))));
               }
               else valpush(env->stack, val(128));

    case 'O' : output(stdout,a);
               break;

    case '`' : valdrop(env->stack);
               if (isstring(a)) {
                   val_t buf;
                   FILE *fp;
                   buf = valbuf(500);
                   fp = popen(valtostr(a),"r");
                   valbufreadfile(buf,fp);
                  _dbgtrc("Shell Result (%d):\n%s\n",valcount(buf),valtostr(buf));
                   pclose(fp);
                   valpush(env->stack,buf);
               }
               break;
  }
}

void dofunc_2(eval_env_t *env, char f)
{
  val_t a, b;
  int32_t i32_r;

  a = valtop(env->stack,-2);
  b = valtop(env->stack);
  valdrop(env->stack,2);
               
  switch (f) {

    case '+' : if (isstring(a)) {
                 char *p;
                 char num[20];
                 if (isstring(b)) {
                   p = valtostr(b);
                 } 
                 else {
                   sprintf(num,"%d",valtoint(b));
                   p = num;
                 }
                 val_t buf = addbuf(env);
                 valbufcat(buf,valtostr(a));
                 valbufcat(buf,p);
                 valpush(env->stack,buf);
               }
               else valpush(env->stack, val(valtoint(a) + valtoint(b)));
               break;

    case '-' : valpush(env->stack, val(valtoint(a) - valtoint(b)));
               break;

    case '*' : valpush(env->stack, val(valtoint(a) * valtoint(b)));
               break;

    case '/' : valpush(env->stack, val(valtoint(a) / valtoint(b)));
               break;

    case '%' : valpush(env->stack, val(valtoint(a) % valtoint(b)));
               break;

    case '^' : { int32_t base = val(valtoint(a));
                 int32_t expn = val(valtoint(b));
                 int32_t powr = 1;
                 for (int k=0; k<expn;k++) powr *= base;
                 valpush(env->stack, val(powr));
               }
               break;

    case ';' : valpush(env->stack,b);
               break;

    case '<' : i32_r = valcmp(a,b);
               valpush(env->stack, (i32_r < 0)?valtrue:valfalse);
               break;

    case '>' : i32_r = valcmp(a,b);
               valpush(env->stack, (i32_r > 0)?valtrue:valfalse);
               break;

    case '?' : i32_r = valcmp(a,b);
               valpush(env->stack,(i32_r == 0)?valtrue:valfalse);
               break;
  }
}

char *cast2str(val_t a, char *num)
{
    if (a == valtrue)  return T_string;
    if (a == valfalse) return F_string;
    if (a == valnil)   return N_string;
    if (isstring(a))   return valtostr(a);
    
    sprintf(num,"%d",valtoint(a));
    return num;
}

void dofunc_3(eval_env_t *env, char f)
{
  val_t a, b, c;
  char num[20];
  char *p;
  dbgtrc("F3: %c",f);
  if (f == 'G') {
    val_t buf;
    int32_t from;
    int32_t len;
    int32_t l;

    a = valtop(env->stack,-3);
    b = valtop(env->stack,-2);
    c = valtop(env->stack);
    valdrop(env->stack,3);

    p = cast2str(a, num);

    dbgtrc("GET A:%s",p);
    from = valtoint(b);
    len = valtoint(c);
    l = strlen(p);
    if (from >= l || *p == '\0') {
      valpush(env->stack, valnilstr);
      return;
    }
    if (len <=0) len = l;
    buf = addbuf(env);
    valbufcpy(buf,p+from,0,len);
    valpush(env->stack, buf);
  }
  else die("unkonw function");
}

void dofunc_4(eval_env_t *env, char f)
{
  val_t a, b, c, d;
  char num1[20];
  char num2[20];
  char *p1;
  char *p2;
  dbgtrc("F4: %c",f);
  if (f == 'S') {
    val_t buf;
    int32_t from;
    int32_t len;
    int32_t l;

    a = valtop(env->stack,-4);
    b = valtop(env->stack,-3);
    c = valtop(env->stack,-2);
    d = valtop(env->stack);   
    valdrop(env->stack,4);

    p1 = cast2str(a, num1);
    dbgtrc("SET A:%s",p1);
    l = strlen(p1);

    from = valtoint(b);
    if (from < 0) from = 0;
    if (from > l) from = l;

    len = valtoint(c);
    if ((len < from) || (from+len) > l) len = l - from;

    p2 = cast2str(d, num2);
    dbgtrc("SET D:%s",p2);

    buf = addbuf(env);
 
    valbufcpy(buf,p1,0,from);
    dbgtrc("SET1: %s",valtostr(buf));
    valbufcat(buf,p2);
    dbgtrc("SET2: %s",valtostr(buf));
    valbufcat(buf,p1+from+len);
    dbgtrc("SET3: %s",valtostr(buf));

    valpush(env->stack, buf);
  }
  else die("unkonw function");
}

val_t string_const(val_t stack, char *start, int32_t len)
{
  start[len] = '\0';
  return val(start);
}

#define NODE_OFFSET 1

val_t kneval(ast_t astcur)
{

  int32_t curnode = astroot(astcur);
  val_t top ;
  char *start;

  eval_env_t env;

  env.stack     = valvec(100);
  env.vars      = valnil;
  env.vars_val  = valnil;
  env.bufs      = valnil;
  env.bufs_free = valnil;

  srand(time(0));

  if (astaux(astcur) == NULL) return -1;
  env.vars = *((val_t *)astaux(astcur));
  
  if (valisvec(env.vars)) {
    env.vars_val = valvec(valcount(env.vars));
    val_t *a = valarray(env.vars_val);
    if (a == NULL) die("Unexpected!");
    for (int k=0; k<valcount(env.vars); k++) 
      a[k] = valnil;
  }

  while (curnode != ASTNULL ) {
    if (astisnodeentry(astcur,curnode)) {
      start = astnodefrom(astcur,curnode);
     _dbgtrc("NODE: %d",curnode);
      if (astnodeis(astcur,curnode,number)) {
        dbgtrc("NUM: %d", atoi(start));
        valpush(env.stack,val(atoi(start)));
      }
      else if (astnodeis(astcur,curnode,string)) {
        valpush(env.stack,string_const(env.stack,start,astnodelen(astcur,curnode)));
      }
      else if (astnodeis(astcur,curnode,variable)) {
        int v = findvar(env.vars,start);
        dbgtrc("VAR: %d (%.4s)", v,start);
        valpush(env.stack,valget(env.vars_val,val(v)));
      }
      else if (astnodeis(astcur,curnode,varref)) {
        int v = findvar(env.vars,start);
        dbgtrc("VARREF: %d (%.4s)", v,start);
        valpush(env.stack,val(v));
      }
      else if (astnodetag(astcur,curnode) >= 0xF0) { // It's a function!
        dbgtrc("FNC: %c ", *start);
        switch (astnodetag(astcur,curnode)) {
          case 0xF0: dofunc_0(&env,*start); break;
          case 0xF1: dofunc_1(&env,*start); break;
          case 0xF2: dofunc_2(&env,*start); break;
          case 0xF3: dofunc_3(&env,*start); break;
          case 0xF4: dofunc_4(&env,*start); break;
          default:   die("unknown function");
        }
      }
      else if (astnodeis(astcur,curnode,assign)) {
        if (env.vars_val == valnil) die("No variable defined/referenced");
        dbgtrc("ASSIGN:");
        assignvar(&env);
      }
      else if (astnodeis(astcur,curnode,while_check)) {
        val_t a = valtop(env.stack);
        dbgtrc("WHILE_CHECK drop: %lX",a);
        if (isfalse(a)) curnode = astlast(astcur,curnode);
        else valdrop(env.stack);
      }
      else if (astnodeis(astcur,curnode,while_end)) {
        val_t a = valtop(env.stack);
        valdrop(env.stack);
        dbgtrc("WHILE_end drop: %lX",a);
        curnode = astup(astcur,curnode);
      }
      else if (astnodeis(astcur,curnode, if_then, and_check)) {
        val_t a = valtop(env.stack);
        valdrop(env.stack);
        if (isfalse(a)) {
          curnode = astright(astcur,curnode);
          curnode = astright(astcur,curnode);
        }
      }
      else if (astnodeis(astcur,curnode,if_else)) {
          curnode = astright(astcur,curnode);
          curnode = astright(astcur,curnode);
      }
      else if (astnodeis(astcur,curnode, or_check)) {
        val_t a = valtop(env.stack);
        valdrop(env.stack);
        if (!isfalse(a)) {
          curnode = astright(astcur,curnode);
          curnode = astright(astcur,curnode);
        }
      }
      else if (astnodeis(astcur,curnode,block)) {
        val_t addr = valconst(NODE_OFFSET,astdown(astcur,curnode));
        curnode = astright(astcur,curnode);
        valpush(env.stack,addr);
      }
      else if (astnodeis(astcur,curnode,block_ret)) {
        val_t a = valtop(env.stack,-2);
        val_t b = valtop(env.stack);
        dbgtrc("BLOCK RETURN: %lX",a);
        valdrop(env.stack,2);
        valpush(env.stack,b);
        if (!valisconst(NODE_OFFSET,a)) die("invalid return");
        curnode = valtoint(a);
      }
      else if (astnodeis(astcur,curnode,call)) {
        val_t a = valtop(env.stack);
        if (!valisconst(NODE_OFFSET,a)) die("invalid block");
        valdrop(env.stack);
        val_t r = valconst(NODE_OFFSET,curnode);
        valpush(env.stack,r);
        dbgtrc("CALL %lX ret: %lX",a,r);
        curnode = valtoint(a);
      }
    }
//    else { // NODE EXIT
//      if (astnodeis(astcur,curnode,while_end)) {
//        val_t a = valtop(env.stack);
//        valdrop(env.stack);
//        dbgtrc("WHILE_end last drop: %lX",a);
//      }
//    }
    curnode = astnext(astcur,curnode);
  }
  top = valtop(env.stack);
  
  dbgtrc("FINAL DEPTH: %d",valcount(env.stack));

  valfree(env.vars_val);
  valfree(env.stack);
  
  for (val_t p = env.bufs; p != valnil; p=env.bufs ) {
    env.bufs = valaux(p);
    valfree(p);
  }
  for (val_t p = env.bufs_free; p != valnil; p=env.bufs_free ) {
    env.bufs_free = valaux(p);
    valfree(p);
  }

  return top;
}