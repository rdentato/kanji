#ifndef VAL_H_VER
#define VAL_H_VER 0x0000001A

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// ## REFERENCES
//
// "Crafting Interpreters" by Robert Nystrom 
// https://craftinginterpreters.com/optimization.html
//
// "NaN boxing or how to make the world dynamic"
// https://piotrduperas.com/posts/nan-boxing
//
// Nanobox implementation by Viktor Söderqvist 
// https://github.com/zuiderkwast/nanbox/blob/master/nanbox.h
//
// Intel® 64 and IA-32 Architectures Software Developer’s Manual vol 1.
// https://xem.github.io/minix86/manual/intel-x86-and-64-manual-vol1/o_7281d5ea06a5b67a.html
// (see table 4.3 on pages 90)

//                  __ Quiet NaN
//                 /__ QNaN Floating-Point Indefinite have 0 here
//                //   
// x111 1111 1111 11xx FF FF FF FF FF FF
//                     \_______________/ <- Payload (48 bits)
//

typedef uint64_t val_t;

#define VAL_MASK      0xFFFF000000000000llu
#define VAL_NANMASK   0x7FFC000000000000llu

#define VAL_PTRMASK   0x0000FFFFFFFFFFFFllu
#define VAL_INTMASK   0x00000000FFFFFFFFllu
#define VAL_INTTYPE   0xFFFFFFFF00000000llu
#define VAL_BOLTYPE   0xFFFFFFFFFFFFFFFEllu

#define valfalse      0x7FFE800000000000llu
#define valtrue       0x7FFE800000000001llu
#define valnil        0x7FFE400000000000llu
#define valempty      0x7FFE100000000002llu
#define valOK         0x7FFE100000000003llu
#define valerror      0x7FFE1000FFFFFFFFllu

#define valconst(x)  (0x7FFE000100000000llu | (uint32_t)(x))
#define valmark(x)   (0x7FFE000200000000llu | (uint32_t)(x))
#define valauxint(x) (0x7FFE000300000000llu | (uint32_t)(x))
#define valextint(x) (0x7FFE000400000000llu | (uint32_t)(x))
#define valrefint(x) (0x7FFE000500000000llu | (uint32_t)(x))
#define valenum(x)   (0x7FFE000600000000llu | (uint32_t)(x))

#define VAL_INT       0x7FFE000000000000llu
#define VAL_PTR       0x7FFF000000000000llu
#define VAL_MAP       0xFFFC000000000000llu
#define VAL_VEC       0xFFFD000000000000llu
#define VAL_BUF       0xFFFE000000000000llu
#define VAL_STR       0xFFFF000000000000llu

#define VAL_argcnt(x1,x2,x3,x4,xN, ...) xN
#define VAL_argn(...)       VAL_argcnt(__VA_ARGS__,4,3,2,1,0)
#define VAL_argjoin(x,y)    x ## y
#define VAL_argcat(x,y)     VAL_argjoin(x,y)
#define VAL_varargs(f,...)  VAL_argcat(f, VAL_argn(__VA_ARGS__))(__VA_ARGS__)

#define valisdbl(x)  (((x) & VAL_NANMASK) != VAL_NANMASK)
#define valisint(x)  (((x) & VAL_INTTYPE) == VAL_INT)
#define valisbuf(x)  (((x) & VAL_MASK) == VAL_BUF)
#define valisptr(x)  (((x) & VAL_MASK) == VAL_PTR)
#define valisvec(x)  (((x) & VAL_MASK) == VAL_VEC)
#define valisstr(x)  (((x) & VAL_MASK) == VAL_STR)

#define valisbool(x) (((x) & VAL_BOLTYPE) == valfalse)

#define VALDBL      0x3
#define VALCST      0x7FFD
#define VALINT      0x7FFE
#define VALPTR      0x7FFF
#define VALMAP      0xFFFC
#define VALVEC      0xFFFD
#define VALBUF      0xFFFE
#define VALSTR      0xFFFF
#define VALNIL      0x1
#define VALBOOL     0x2

static inline int VALTYPE(val_t v)
{
  if (valisdbl(v)) return VALDBL;
  if (valisbool(v)) return VALBOOL;
  if (v == valnil) return VALNIL;
  return (int)((v)>>48); 
}

static inline double valtodbl(val_t v) 
{ double d; memcpy(&d,&v,8); return d; }

static inline int32_t valtoint(val_t v)
{ return (int32_t)(v & VAL_INTMASK); }

static inline void *valtoptr(val_t v)
{ return (void *)((uintptr_t)(v & VAL_PTRMASK)); }

char *valtostr(val_t v);

static inline val_t val_fromint(int v)               { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromchar(char v)             { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromshort(short v)           { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromlong(long v)             { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromuint(unsigned int v)     { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromuchar(unsigned char v)   { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromushort(unsigned short v) { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromulong(unsigned long v)   { uint64_t ret = v; return VAL_INT | (ret & VAL_INTMASK);}
static inline val_t val_fromdbl(double v)            { val_t n; memcpy(&n,&v,8); return n; }
static inline val_t val_fromflt(float f)             { double v = (double)f; val_t n; memcpy(&n,&v,8); return n; }
static inline val_t val_fromptr(void *v)             { return VAL_PTR | ((uintptr_t)v & VAL_PTRMASK);}
static inline val_t val_fromstr(void *v)             { return VAL_STR | ((uintptr_t)v & VAL_PTRMASK);}

#define val(x) _Generic( (x),   int: val_fromint, \
                               char: val_fromchar, \
                              short: val_fromshort, \
                               long: val_fromlong, \
                       unsigned int: val_fromuint, \
                      unsigned char: val_fromuchar, \
                     unsigned short: val_fromushort, \
                      unsigned long: val_fromulong, \
                             double: val_fromdbl, \
                              float: val_fromflt, \
                             char *: val_fromstr, \
                             void *: val_fromptr) (x)

#define valbool(x) ((x)?valtrue:valfales)

  val_t valvec(int32_t sz);
  val_t valbuf(int32_t sz);

  val_t valset(val_t v, val_t i, val_t x);
  val_t valget(val_t v, int32_t i);
  val_t valpush(val_t v, val_t x);

#define valdrop(...) VAL_varargs(val_drop,__VA_ARGS__)
#define val_drop1(v) val_drop2(v,-1)
  val_t val_drop2(val_t v, int32_t n);

#define valtop(...) VAL_varargs(val_top,__VA_ARGS__)
#define val_top1(v)   val_top2(v,-1)
  val_t val_top2(val_t v, int32_t n);

  val_t valenq(val_t q, val_t x);

#define valdeq(...) VAL_varargs(val_deq,__VA_ARGS__)
#define val_deq1(v) val_deq2(v,1)
  val_t val_deq2(val_t v, int32_t n);

#define valhead(...) VAL_varargs(val_head,__VA_ARGS__)
#define val_head1(v) val_head2(v,0)
  val_t val_head2(val_t q, int32_t n);

#define valtail(...) valtop(__VA_ARGS__)

   void valclear(val_t v);

int32_t valcount(val_t v);
int32_t valsize(val_t v);

  val_t valfree(val_t v);
  val_t valresize(val_t v, int32_t sz);

    int valcmp(val_t a, val_t b);
  val_t valadd(val_t a, val_t b);
  val_t valsub(val_t a, val_t b);
  val_t valmul(val_t a, val_t b);
  val_t valdiv(val_t a, val_t b);
  val_t valmod(val_t a, val_t b);
  val_t valand(val_t a, val_t b);
  val_t valor(val_t a, val_t b);
  val_t valxor(val_t a, val_t b);

#ifdef VAL_MAIN

//  --------------------------------------------------------
//  ooo        ooooo        .o.        ooooo  ooooo      ooo 
//  `88.       .888'       .888.       `888'  `888b.     `8' 
//   888b     d'888       .8"888.       888    8 `88b.    8  
//   8 Y88. .P  888      .8' `888.      888    8   `88b.  8  
//   8  `888'   888     .88ooo8888.     888    8     `88b.8  
//   8    Y     888    .8'     `888.    888    8       `888  
//  o8o        o888o  o88o     o8888o  o888o  o8o        `8  
//  --------------------------------------------------------

#define val_cmp_(x,y) ((x)<(y)?-1:((x)==(y))?0:1)
int valcmp(val_t a, val_t b)
{
  if (a == b) return 0;
  switch(VALTYPE(a)) {
    case VALINT: if (valisint(b)) return val_cmp_(valtoint(a), valtoint(b));
                 if (valisdbl(b)) return val_cmp_(valtoint(a), valtodbl(b));
                 break;

    case VALDBL: if (valisint(b)) return val_cmp_(valtodbl(a), valtoint(b));
                 if (valisdbl(b)) return val_cmp_(valtodbl(a), valtodbl(b));
                 break;

    case VALSTR: { char *pa, *pb;
                   if ((pa = valtostr(a)) && (pb = valtostr(b))) return strcmp(pa,pb);
                 } 
                 break;
  }
  return -1;
}

val_t valadd(val_t a, val_t b) 
{ 
  switch(VALTYPE(a)) { 
    case VALINT: if (valisint(b)) return val((valtoint(a) + valtoint(b)));
                 if (valisdbl(b)) return val((valtoint(a) + valtodbl(b)));
                 if (valisptr(b)) return val((void *)((valtoint(a) + (char *)valtoptr(b))));
                 break;
    case VALDBL: if (valisint(b)) return val((valtodbl(a) + valtoint(b)));
                 if (valisdbl(b)) return val((valtodbl(a) + valtodbl(b)));
                 if (valisptr(b)) return val((void *)(((int)valtodbl(a) + (char *)valtoptr(b))));
                 break;
    case VALPTR: if (valisint(b)) return val((void *)(((char *)valtoptr(a)) + valtoint(b)));
                 if (valisdbl(b)) return val((void *)(((char *)valtoptr(a)) + (int)valtodbl(b)));
                 break;
  }
  return valnil;
} 

val_t valsub(val_t a, val_t b)
{ 
  switch(VALTYPE(a)) { 
    case VALINT: if (valisint(b)) return val((valtoint(a) - valtoint(b)));
                 if (valisdbl(b)) return val((valtoint(a) - valtodbl(b)));
                 break;
    case VALDBL: if (valisint(b)) return val((valtodbl(a) - valtoint(b)));
                 if (valisdbl(b)) return val((valtodbl(a) - valtodbl(b)));
                 break;
    case VALPTR: if (valisint(b)) return val((void *)(((char *)valtoptr(a)) - valtoint(b)));
                 if (valisdbl(b)) return val((void *)(((char *)valtoptr(a)) - (int)valtodbl(b)));
                 if (valisptr(b)) return val((int)(((char *)valtoptr(a)) - (char *)valtoptr(b)));
                 break;
  }
  return valnil;
} 

#define val_op_mul(name,op_)  \
val_t val##name(val_t a, val_t b) \
{ \
  switch(VALTYPE(a)) { \
    case VALINT: if (valisint(b)) return val((valtoint(a) op_ valtoint(b))); \
                 if (valisdbl(b)) return val((valtoint(a) op_ valtodbl(b))); \
                 break;\
    case VALDBL: if (valisint(b)) return val((valtodbl(a) op_ valtoint(b))); \
                 if (valisdbl(b)) return val((valtodbl(a) op_ valtodbl(b))); \
                 break;\
  } \
  return valnil;   \
} 

val_op_mul(mul,*)
val_op_mul(div,/)

#define val_int_op(name,op_) \
val_t val##name(val_t a, val_t b)\
{\
  switch(VALTYPE(a)) {\
    case VALINT: if (valisint(b)) return val(valtoint(a) op_ valtoint(b));\
                 if (valisdbl(b)) return val(valtoint(a) op_ (int32_t)valtodbl(b));\
                 break;\
    case VALDBL: if (valisint(b)) return val((int32_t)valtodbl(a) op_ valtoint(b));\
                 if (valisdbl(b)) return val((int32_t)valtodbl(a) op_ (int32_t)valtodbl(b));\
                 break;\
  }\
  return valnil; \
}

val_int_op(mod,%)
val_int_op(and,&)
val_int_op(or,|)
val_int_op(xor,^)

typedef struct val_info_s {
    void *arr;
  int32_t size;
  int32_t count;
  int32_t first;
  int32_t prv_sz;
  int32_t cur;
  int32_t refcnt;
} *val_info_t;

val_t valfree(val_t v)
{
  switch (VALTYPE(v)) {
    case VALBUF:
    case VALVEC: { val_info_t vv = valtoptr(v);
                   free(vv->arr);
                   vv->size = vv->count = vv->cur = vv->refcnt = 0;
                   vv->arr = NULL;
                   free(vv);
                 }
                 break;
  }

  return valnil;
}

static int val_makeroom(val_info_t vv, int32_t i, int32_t esz) 
{
  uint32_t new_sz;
  uint32_t prv_sz;
  uint32_t tmp_sz;
  val_t *new_arr;
  if ( i >= vv->size) {
    new_sz = vv->size;
    prv_sz = vv->prv_sz;
    while (new_sz <= i) { // Grow by fibonacci seq 0,4,4,8,12,20, ...
      tmp_sz  = new_sz;
      new_sz += prv_sz;
      prv_sz  = tmp_sz;
    }
    // dbgtrc("i: %d prv: %d new: %d",i,vv->size, new_sz);

    new_arr = realloc(vv->arr, new_sz * esz);
    if (new_arr == NULL) return 0; // ERR
    vv->arr = new_arr;
    vv->size = new_sz;
    vv->prv_sz = prv_sz;
  }
  return 1; // OK
}

val_t valresize(val_t v, int32_t sz)
{
  
  if (valisvec(v)) {
    val_makeroom(valtoptr(v), sz, sizeof(val_t));
    return v;
  }

  if (valisbuf(v)) {
    val_makeroom(valtoptr(v), sz, 1);
    return v;
  }

  return valnil;
}

int32_t valsize(val_t v)
{
  val_info_t vv;
  dbgtrc("TYPE: %X",VALTYPE(v));
  switch(VALTYPE(v)) {
    case VALBUF: 
    case VALVEC: vv = valtoptr(v);
                 return vv ? vv->size : 0;
                 
    default: dbgtrc("SZ OF: %lX",v);
  }
  return 0;
}

int32_t valcount(val_t v)
{
  switch (VALTYPE(v)) {
    case VALBUF:
    case VALVEC: { val_info_t vv = valtoptr(v);
                   return vv ? vv->count - vv->first : 0;
                 }
  }
  return 0;
}

static val_t val_vec(int32_t sz, int32_t esz, val_t type)
{
  val_info_t vv;
  val_t vvec = valnil;

  vv = malloc(sizeof(struct val_info_s));
  if (vv == NULL) return valnil;

  vv->arr = NULL;
  vv->size = 0;
  vv->first = 0;
  vv->prv_sz = 4; // Start of Fibonacci seq 0,4,...
  vv->count = 0;
  vv->cur = 0;
  vv->refcnt = 1;

  if ((sz > 0) && !val_makeroom(vv,sz,esz)) { 
    vv->size = 0;
    free(vv);
    return valnil;
  }

  vvec = type | (((uintptr_t)vv) & VAL_PTRMASK);
  return vvec;
}

val_t valvec(int32_t sz)
{ return val_vec(sz, sizeof(val_t), VAL_VEC); }

val_t valbuf(int32_t sz)
{ return val_vec(sz, 1, VAL_BUF); }

char *valtostr(val_t v)
{ 
  void *p = valtoptr(v);
  switch (VALTYPE(v)) {
    case VALBUF: 
       return (char *)(((val_info_t)p)->arr);
    case VALPTR:
    case VALSTR:
       return (char *)p;
  }
  return NULL;
}

val_t valset(val_t v, val_t i, val_t x)
{ 
  int32_t ii = -1;
  // dbgtrc("%X",VALTYPE(v));
  switch (VALTYPE(v)) {
    case VALBUF: { val_info_t vv;
                   if ((vv = valtoptr(v)) == NULL) return valerror;
                  
                   if (valisint(i)) ii = valtoint(i);
                   else if (i == valnil) ii = -1;
                   else return valerror;
                    
                   if (ii < 0) ii = vv->count;
                   // dbgtrc("valset [%d]",ii);
                   if (val_makeroom(vv,ii+4+1,1)) {
                     char *s = (char *)(vv->arr);
                     uint32_t ch = (uint32_t)valtoint(x);

                     if (ch && 0xFF000000) s[ii++] = (ch >> 24) & 0xFF;
                     if (ch && 0x00FF0000) s[ii++] = (ch >> 16) & 0xFF;
                     if (ch && 0x0000FF00) s[ii++] = (ch >> 8) & 0xFF;
                     s[ii++] = ch & 0xFF;
                     if (ii >= vv->count) { 
                       vv->count=ii+1;
                       s[vv->count] = '\0';
                     }
                     return val(vv->count);
                   }
                 }
                 break;

    case VALVEC: { val_info_t vv;
                   if ((vv = valtoptr(v)) == NULL) return valerror;
                  
                   if (valisint(i)) ii = valtoint(i);
                   else if (i == valnil) ii = -1;
                   else return valerror;
                    
                   if (ii < 0) ii = vv->count;
                   // dbgtrc("valset [%d]",ii);
                   if (val_makeroom(vv,ii,sizeof(val_t))) {
                     ((val_t *)(vv->arr))[ii] = x;
                     if (ii >= vv->count) vv->count=ii+1;
                     return val(vv->count-vv->first);
                   }
                 }
                 break;
  }

  return valerror;
}

val_t valget(val_t v, int32_t i)
{ 
  val_info_t vv;
  if (!valisvec(v) || (vv = valtoptr(v)) == NULL) return valnil;
  if (vv->count <= vv->first) { vv->count = vv->first = 0; return valnil; }
  if (i < 0) i = vv->count+i;
  if (i < vv->first || vv->count <= i) return valnil;
  return ((val_t*)(vv->arr))[i];
}

void valclear(val_t v)
{
  switch (VALTYPE(v)) {
    case VALBUF:
    case VALVEC: { val_info_t vv;
                   if ((vv = valtoptr(v)) != NULL) {
                     vv->count = vv->first = 0;
                   }
                 }
                 break;
  }
  return;
}


val_t valpush(val_t v, val_t x)
{
  return valset(v,valnil,x);
}

val_t val_top2(val_t v, int32_t n)
{
  if (n>0) n=-n;
  return valget(v,n);
}

val_t val_drop2(val_t v, int32_t n)
{
  val_info_t vv;
  if (!valisvec(v) || (vv = valtoptr(v)) == NULL) return valerror;
  if (n<0) n = -n;
  vv->count -= n;
  if (vv->count <= vv->first) { vv->count = vv->first = 0; }
  return val(vv->count - vv->first);
}

val_t valenq(val_t q, val_t x)
{
  val_info_t vv;
  if (!valisvec(q) || (vv = valtoptr(q)) == NULL) return 0;

  if ((vv->count >= vv->size) && (vv->first >= (vv->size / 2))) {
    memmove(vv->arr, vv->arr+(vv->first * sizeof(val_t)), (vv->count - vv->first) * sizeof(val_t));
    vv->count -= vv->first; vv->first = 0;
  }
  
  return valset(q,valnil,x);
}

val_t val_deq2(val_t q, int32_t n) 
{
  val_info_t vv;
  if (!valisvec(q) || (vv = valtoptr(q)) == NULL) return valerror;
  if (n<0) n = -n;
  vv->first += n; 
  if (vv->count <= vv->first) { vv->count = vv->first = 0; }
  return val(vv->count - vv->first);
}

val_t val_head2(val_t q, int32_t n)
{
  val_info_t vv;
  if (!valisvec(q) || (vv = valtoptr(q)) == NULL) return 0;
  if (n<0) n=-n;
  n = vv->first+n;
  if (vv->count <= vv->first) {
    vv->count = vv->first = 0; return valnil; 
  }
  return ((val_t *)(vv->arr))[n] ;
}

val_t valmap(int32_t sz)
{
  
  return valnil;
}

int32_t valmapset(val_t map, val_t k, val_t v)
{
  return 0;
}

val_t valmapget(val_t map, val_t k)
{
  return valnil;
}

#endif // VAL_MAIN
#endif // VAL_VERSION_H