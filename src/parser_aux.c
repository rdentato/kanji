/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

  #include "dbg.h"
  #include "val.h"
  #include "fname.h"

  #define MAXLOOPNEST 16
  static int32_t loopstk[MAXLOOPNEST];
  static int32_t loopstk_cnt = 0;
  #define loopstk_push(x) (loopstk[loopstk_cnt++] = (x))
  #define loopstk_drop() (--loopstk_cnt)
  #define loopstk_top()  (loopstk[loopstk_cnt-1])

  static int32_t infunc = 0;
  static int32_t mainnode = 0;

  typedef struct {
    uint64_t cname;
    int32_t node;
  } func_table_slot;

  static int32_t round2(int32_t n) {
    return (n+1) & 0xFFFFFFFE;
  }

  static func_table_slot *func_table = NULL;
  static int func_table_count  = 0;
  static int func_table_max    = 0;

  static int addfunc(char *fname, int32_t node)
  {
    uint64_t cname=0;

    cname = encoded_fname(fname);
    if (cname == INVALID_FNAME) return 0;

    if (func_table_count >= func_table_max) {
      func_table_max += (func_table_max / 2) + 4;
      func_table_max += func_table_max & 1;
      func_table = realloc(func_table, func_table_max * sizeof(func_table_slot));
      assert(func_table);
    }
    func_table[func_table_count].cname = cname;
    func_table[func_table_count].node = node;
    func_table_count++;
    return 1;
  }

  static char *const_table = NULL;
  static int const_table_cur = 0;
  static int const_table_max = 0;

  static int32_t addconst(void *ptr, int32_t size)
  {
    int32_t old_cur = const_table_cur;
    size = (size+7) & 0xFFFFFFF8; // ensure is a multiple of 8
    if (const_table_cur + size >= const_table_max) {
      const_table_max += (const_table_max / 2) + size;
      const_table_max += const_table_max & 1;
      const_table = realloc(const_table, const_table_max);
      assert(const_table);
    }
    memcpy(const_table+const_table_cur,ptr, size);
    const_table_cur += size;
    return old_cur;
  }

/**
 * It takes a function name,
 * encodes it, and adds it to the constant pool
 * 
 * @param fname The name of the function being called.
 * @param fname_end the end of the function name
 * @param node the node number of the function
 * 
 * @return The index of the constant in the constant table.
 */
  static int32_t addcall(char *fname, char *fname_end, int32_t node)
  {
    uint64_t cname;
    cname = encoded_fname(fname);
    dbgtrc("%016lX %03d '%.*s'",cname,node,(int)(fname_end - fname), fname);
    return addconst(&cname,sizeof(uint64_t));
  }

  #define CHR_ZERO 0x80000000

  static int32_t addstr(char *s, int32_t len)
  {
    int32_t slot;
    int32_t old_slot;

   _dbgtrc("STR: %d %.*s",len,len,s);

    if (len == 0) return CHR_ZERO;
    if (len == 1) return -(*s);

    old_slot = const_table_cur;
    slot = addconst(s,len+1);
    const_table[slot+len] ='\0';
    
    // Clean up escape sequences 

    char *src = const_table+slot;
    char *dst = src;

    while (*src) {
      
      if (*src == '\\') { /* Cleaning up escape sequences. */
        src++;
        switch (*src) {
          case  '1': case  '2': case  '3': case  '4':
          case  '5': case  '6': case  '7': case  '8': case  '9':
                     *dst = strtol(src,&src,10)   & 0xFF; src--; break;
          case  '0': *dst = strtol(src,&src,8)    & 0xFF; src--; break;
          case  'x': *dst = strtol(src+1,&src,16) & 0xFF; src--; break;
          case  'b': *dst = strtol(src+1,&src,2)  & 0xFF; src--; break;
          case  'n': *dst = '\n'; break;
          case  'r': *dst = '\r'; break;
          case  't': *dst = '\t'; break;
          case  '.': break;
          default  : *dst = *src;
        }
      } 
      else *dst = *src;
      src++; dst++;
    }
   *dst = '\0';
    dst = const_table+slot;

    if (dst[0] == '\0') { const_table_cur = old_slot; return CHR_ZERO; }
    if (dst[1] == '\0') { const_table_cur = old_slot; return -(dst[0]); }
    if (dst[2] == '\0') { const_table_cur = old_slot; return -(dst[1] << 8  | dst[0]); }
    if (dst[3] == '\0') { const_table_cur = old_slot; return -(dst[2] << 16 | dst[1] << 8 | dst[0]); }

    return slot;
  }

  static void cleanfunc()
  {
    if (func_table) free(func_table);
    func_table = NULL;
    func_table_max = 0;
    func_table_count = 0;
  }

  static int ft_cmp(const void *a, const void *b)
  {
    const func_table_slot *slot_a = a;
    const func_table_slot *slot_b = b;
    if (slot_a->cname == slot_b->cname) return 0;
    if (slot_a->cname <  slot_b->cname) return -1;
    return 1;
  }

  static int sortfunc()
  {
    qsort(func_table,func_table_count,sizeof(func_table_slot),ft_cmp);
    for (int k=1; k<func_table_count; k++) {
      if (func_table[k].cname == func_table[k-1].cname) {
        return 0;
      }
    }
    return 1;
  }

/**
 * > Search the function table for the function name, and return the node number if found, or -1 if not
 * found
 * 
 * @param fname the name of the function to search for
 * 
 * @return The node number of the function.
 */
  static int32_t searchfunc(char *fname)
  {
    uint64_t cname = encoded_fname(fname);
   
    int i = 0;
    int j = func_table_count-1;
    int m;

    while (i<=j) {
      m = (i+j)/2;
      if (func_table[m].cname == cname) return func_table[m].node;
      if (func_table[m].cname <  cname) {
        i = m+1;
      }
      else {
        j = m-1;
      }
    }
    return -1;
  }
  
  skpfunc(op) {
    char *p;
    int32_t n = 0;

    static char *stk_ops = "and\0\0\0"    //  1
                           "drop\0\0"     //  2
                           "dup\0\0\0"    //  3   
                           "empty\0"      //  4 
                           "false\0"      //  5  
                           "nip\0\0\0"    //  6
                           "nop\0\0\0"    //  7
                           "not\0\0\0"    //  8
                           "or\0\0\0\0"   //  9
                           "over\0\0"     // 10
                           "pick\0\0"     // 11
                           "roll\0\0"     // 12
                           "rot\0\0\0"    // 13
                           "swap\0\0"     // 14
                           "true\0\0"     // 15
                           "tuck\0\0"     // 16
                           "xor\0\0\0"    // 17
                           "zero\0\0"     // 18
                           "\0\0\0\0\0";

    #define NUM_OF_STK_OPS 18
    #define LEN_OF_STK_OPS 6

    // Check for stack operations.
    if ('a' <= *skpcur && *skpcur <= 'z') {
       int k;
       int i = 0;
       int j = NUM_OF_STK_OPS - 1;
      _dbgtrc("STK: %.4s...",skpcur);

       while (i <= j) { // Binary search within the string
          n = (i+j)/2;
          p = stk_ops + (n * LEN_OF_STK_OPS);
         _dbgtrc("STKOP: %d %.4s",n,p);
          for (k=0; p[k] ; k++) {
             if (skpcur[k] != p[k]) break;
          }
          if (p[k] == '\0' && !isalnum(skpcur[k])) 
            skpreturn(skpcur+k,51+n);
          if (p[k] < skpcur[k])  i = n+1;
          else j = n-1;
       }

       skpreturn(NULL,0);
    }

                //    01234567890123
    static char *ops="!><=+-|&%*/^~";
    const int ops_len = 13;

   _dbgtrc("OP?: %c",skpcur[0]);
    p = ops;
    while (*p && *p != skpcur[0]) p++;
    if (*p == '\0') skpreturn(NULL,0);
   _dbgtrc("OP=: %c",*p);

    n = p - ops  ;
    skpcur++;

    if (n <= 7  && *skpcur == *p )  {skpcur++; n+= ops_len;}
    else if (n <= 3  && *skpcur == '=' ) {skpcur++; n+= 8+ops_len;}

    skpreturn(skpcur,n+1);
  }
