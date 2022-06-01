#include "kanji.h"
#include "kanji_sys.h"

typedef val_t (*kaj_sys_f)(val_t);

typedef struct {
  int64_t fname;
  kaj_sys_f fptr;
} kaj_sys_t;

#include "syslib_fnc.h"
 
static kaj_sys_t kaj_syslst[] = {
  #include "syslib_lst.h"
}; 
 
#define SYS_NUM (sizeof(kaj_syslst) / sizeof(kaj_sys_t))

#define VAL_MAIN
#include "val.h"

#undef  TOK_HLT
#define TOK_HLT 0x00

static kaj_sys_f kaj_checkfname(int64_t fname)
{
  int i, j, m;

  i = 0;
  j = SYS_NUM-1;

  while ( i<=j ){
    m = (i+j)/2;
    if (kaj_syslst[m].fname == fname) return kaj_syslst[m].fptr;
    if (kaj_syslst[m].fname < fname) i=m+1;
    else j = m-1;
  }
  return NULL;
}

int kaj_setreg(kaj_pgm_t pgm, uint8_t reg, val_t input)
{
  if (reg > pgm->max_regs) return ERR_INVALID_REG;
  pgm->lst.regs[reg] = input;
  return ERR_NONE;
}

val_t kaj_getreg(kaj_pgm_t pgm, uint8_t reg)
{
  if (reg > pgm->max_regs) return valnil;
  return pgm->lst.regs[reg];
}

static int push_pgm(kaj_pgm_t pgm, uint32_t val)
{
  if (pgm->pgm_count >= pgm->pgm_size) return ERR_NO_MEMORY;
 _dbgtrc("push pgm: %d %08X",pgm->pgm_count,val);
  pgm->pgm[pgm->pgm_count++] = val;
  return ERR_NONE;
}

// static uint32_t pop_pgm(kaj_pgm_t pgm)
// {
//   if (pgm->pgm_count <= pgm->max_pgm+1) return 0;
//   return pgm->pgm[--pgm->pgm_count];
// }

static int start_JSR(kaj_pgm_t pgm, uint8_t reg, val_t input)
{
  int err=ERR_NONE;
  uint32_t k;
  void *p;
  err = kaj_setreg(pgm, 0, input);

  k = (pgm->cur_ln) << 8 | reg;
  push_pgm(pgm,k);
  p = (void *)&(pgm->pgm[pgm->pgm_count]);
  push_pgm(pgm,0);
  push_pgm(pgm,0);
  memcpy(p,&input,8);
  return err;
}

int kaj_init(kaj_pgm_t pgm, int32_t start, uint8_t reg, val_t input)
{
  if (pgm->lst_type != LST_REGISTERS) return ERR_NOTASSEMBLED_PGM;
  if (reg > pgm->max_regs) return ERR_INVALID_REG;
 
  dbgtrc("sys: %d",(int)SYS_NUM);
  pgm->pgm_count = pgm->max_pgm+1;  // reset pgm stack
  pgm->lst_count = pgm->max_regs+1; // reset regs stack
  pgm->cur_ln = start;
 _dbgtrc("Input: %lX",input);
  start_JSR(pgm,0,input);

  pgm->pgm[pgm->pgm_count-3] &= 0xFF;
  pgm->pgm[pgm->pgm_count-3] |= pgm->max_pgm << 8;

  pgm->pgm_cmpflg = 0;

  return ERR_NONE;
}
static val_t const2str(char *str, val_t v)
{
  if (valisconst(STR_OFFSET,v)) {
    v = val((char *)(str+valtoint(v)));
  }

  return v;
}
int kaj_step(kaj_pgm_t pgm) 
{
  uint32_t op;
  uint8_t reg;
  int64_t fname;
  kaj_sys_f fptr;
  void *p;
  int32_t n;
  val_t v;
  val_t r;

  op = pgm->pgm[pgm->cur_ln++];
  dbgtrc("STEP: %03X %08X",pgm->cur_ln-1,op);
  switch(op & 0xFF) {

    case TOK_HLT: return 0;

    case TOK_STO:
      pgm->lst.regs[(op >> 8) & 0xFF] = pgm->lst.regs[(op >> 16) & 0xFF];
      break;

    case TOK_ST2: 
      n = ((int32_t)op)>>16;
      pgm->lst.regs[(op >> 8) & 0xFF] = val(n);
     _dbgtrc("n2: %d -> %d",n,valtoint(pgm->lst.regs[(op >> 8) & 0xFF]));
      break;

    case TOK_ST4:
      n = (int32_t)(pgm->pgm[pgm->cur_ln++]);
      pgm->lst.regs[(op >> 8) & 0xFF] = val(n);
     _dbgtrc("n: %d -> %d",n,valtoint(pgm->lst.regs[(op >> 8) & 0xFF]));
      break;

    case TOK_ST8:
      memcpy(&v, &(pgm->pgm[pgm->cur_ln]), 8);
      v = const2str(pgm->str,v);
      pgm->lst.regs[(op >> 8) & 0xFF] = v;
      pgm->cur_ln+=2;
      break;

    case TOK_STI:
      n = valtoint(pgm->lst.regs[(op & 0x00FF0000) >> 16]);
      memcpy(&v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
      v = const2str(pgm->str,v);
      pgm->lst.regs[(op >> 8) & 0xFF] = v;
      pgm->cur_ln++;
      break;

    case TOK_STN:
      n = op >> 16;
      memcpy(&v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
      v = const2str(pgm->str,v);
      pgm->lst.regs[(op >> 8) & 0xFF] = v;
      pgm->cur_ln++;
      break;

    case TOK_ADD:
      pgm->lst.regs[(op >> 8) & 0xFF] = valadd(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_SUB:
      pgm->lst.regs[(op >> 8) & 0xFF] = valsub(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_DIV:
      pgm->lst.regs[(op >> 8) & 0xFF] = valdiv(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_MUL:
      pgm->lst.regs[(op >> 8) & 0xFF] = valmul(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_MOD:
      pgm->lst.regs[(op >> 8) & 0xFF] = valmod(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_AND:
      pgm->lst.regs[(op >> 8) & 0xFF] = valand(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_ORR:
      pgm->lst.regs[(op >> 8) & 0xFF] = valor(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_XOR:
      pgm->lst.regs[(op >> 8) & 0xFF] = valxor(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_SHL:
      pgm->lst.regs[(op >> 8) & 0xFF] = valshl(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_SL1:
      pgm->lst.regs[(op >> 8) & 0xFF] = valshl(pgm->lst.regs[(op >> 16) & 0xFF], val((op >> 24) & 0xFF));
      break;

    case TOK_SHR:
      pgm->lst.regs[(op >> 8) & 0xFF] = valshr(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_SR1:
      pgm->lst.regs[(op >> 8) & 0xFF] = valshr(pgm->lst.regs[(op >> 16) & 0xFF], val((op >> 24) & 0xFF));
      break;

    case TOK_ASR:
      pgm->lst.regs[(op >> 8) & 0xFF] = valasr(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_AR1:
      pgm->lst.regs[(op >> 8) & 0xFF] = valasr(pgm->lst.regs[(op >> 16) & 0xFF], val((op >> 24) & 0xFF));
      break;

    case TOK_INT: 
      v = pgm->lst.regs[(op >> 16) & 0xFF];
      if (valisdbl(v)) {
        pgm->lst.regs[(op >> 8) & 0xFF] = val((int32_t)valtodbl(v));
        break;
      }
      if (valisint(v)) {
        pgm->lst.regs[(op >> 8) & 0xFF] = v;
        break;
      }
      break;

    case TOK_NOT:
      pgm->lst.regs[(op >> 8) & 0xFF] = valnot(pgm->lst.regs[(op >> 16) & 0xFF]);
      break;

    case TOK_NEG:
      pgm->lst.regs[(op >> 8) & 0xFF] = valneg(pgm->lst.regs[(op >> 16) & 0xFF]);
      break;

    case TOK_FLT:
      v = pgm->lst.regs[(op >> 16) & 0xFF];
      if (valisdbl(v)) {
        pgm->lst.regs[(op >> 8) & 0xFF] = v;
        break;
      }
      if (valisint(v)) {
        pgm->lst.regs[(op >> 8) & 0xFF] = val((double)valtoint(v));
        break;
      }
      break;

    case TOK_INC:
      v = pgm->lst.regs[(op >> 8) & 0xFF];
      n = op >> 16;
      dbgtrc("inc: %d",n);
      if (valisdbl(v)) {
        pgm->lst.regs[(op >> 8) & 0xFF] = val(valtodbl(v) + (double)n);
        break;
      }
      if (valisint(v)) {
        pgm->lst.regs[(op >> 8) & 0xFF] = val(valtoint(v) + n) ;
        break;
      }
      if (valisstr(v)) {
        char *s = valtoptr(v);
        while ((n > 0) && *s) { n--; s++;}
        pgm->lst.regs[(op >> 8) & 0xFF] = val(s);
        break;
      }
      break;

    case TOK_DEC: 
      v = pgm->lst.regs[(op >> 8) & 0xFF];
      n = op >> 16;
      if (valisdbl(v)) {
        double d = valtodbl(v) - (double)n;
        pgm->lst.regs[(op >> 8) & 0xFF] = val(d);
        if (d == 0.0) pgm->pgm_cmpflg = FLG_EQUAL;
        break;
      }
      if (valisint(v)) {
        int32_t i = valtoint(v) - n;
        pgm->lst.regs[(op >> 8) & 0xFF] = val(i) ;
        if (i == 0) pgm->pgm_cmpflg = FLG_EQUAL;
        break;
      }
      break;

    case TOK_CMP: 
      n = valcmp(pgm->lst.regs[(op >> 8) & 0xFF], pgm->lst.regs[(op >> 16) & 0xFF]);
      pgm->pgm_cmpflg = (1 << (n+1));
      break;

    case TOK_CP2: 
      n = valcmp(pgm->lst.regs[(op >> 8) & 0xFF], val(((int32_t)op) >> 16));
      pgm->pgm_cmpflg = (1 << (n+1));
      break;

    case TOK_CP4:
      n = valcmp(pgm->lst.regs[(op >> 8) & 0xFF], val((int32_t)(pgm->pgm[pgm->cur_ln++])));
      pgm->pgm_cmpflg = (1 << (n+1));
      break;

    case TOK_CP8:
      memcpy(&v, &(pgm->pgm[pgm->cur_ln]), 8);
      v = const2str(pgm->str,v);
      n = valcmp( pgm->lst.regs[(op >> 8) & 0xFF], v);
      pgm->pgm_cmpflg = (1 << (n+1));
      pgm->cur_ln+=2;
      break;

    case TOK_CPI:
      n = valtoint(pgm->lst.regs[(op & 0x00FF0000) >> 16]);
      memcpy(&v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
      v = const2str(pgm->str,v);
     _dbgtrc("R: %lX, V: %lX",pgm->lst.regs[(op >> 8) & 0xFF],*(uint64_t *)(pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2)));
      n = valcmp( pgm->lst.regs[(op >> 8) & 0xFF], v);
      pgm->pgm_cmpflg = (1 << (n+1));
      pgm->cur_ln++;
      break;

    case TOK_CPN:
      n = op >> 16;
      memcpy(&v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
      v = const2str(pgm->str,v);
     _dbgtrc("R: %lX, V: %lX",pgm->lst.regs[(op >> 8) & 0xFF],*(uint64_t *)(pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2)));
      n = valcmp( pgm->lst.regs[(op >> 8) & 0xFF], v);
      pgm->pgm_cmpflg = (1 << (n+1));
      pgm->cur_ln++;
      break;

    case TOK_DTA:
    case TOK_JMP: 
      pgm->cur_ln = op >> 8;
      break;
 
    case TOK_JEQ: 
      if (pgm->pgm_cmpflg & FLG_EQUAL) pgm->cur_ln = op >> 8;
      break;
 
    case TOK_JNE: 
      if (!(pgm->pgm_cmpflg & FLG_EQUAL)) pgm->cur_ln = op >> 8;
      break;
 
    case TOK_JGT: 
      if (pgm->pgm_cmpflg & FLG_GREATER) pgm->cur_ln = op >> 8;
      break;

    case TOK_JGE: 
      if (pgm->pgm_cmpflg & (FLG_EQUAL | FLG_GREATER)) pgm->cur_ln = op >> 8;
      break;

    case TOK_JLT: 
      if (pgm->pgm_cmpflg & FLG_LESSER) pgm->cur_ln = op >> 8;
      break;

    case TOK_JLE: 
      if (pgm->pgm_cmpflg & (FLG_EQUAL | FLG_LESSER)) pgm->cur_ln = op >> 8;
      break;

    case TOK_ZLE:
      if (pgm->pgm_cmpflg & (FLG_EQUAL | FLG_LESSER)) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_ZMP: 
      pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;
 
    case TOK_ZEQ: 
      if (pgm->pgm_cmpflg & FLG_EQUAL) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];;
      break;
 
    case TOK_ZNE: 
      if (!(pgm->pgm_cmpflg & FLG_EQUAL)) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;
 
    case TOK_ZGT: 
      if (pgm->pgm_cmpflg & FLG_GREATER) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_ZGE: 
      if (pgm->pgm_cmpflg & (FLG_EQUAL | FLG_GREATER)) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_ZLT: 
      if (pgm->pgm_cmpflg & FLG_LESSER) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_ARG:
      reg = (op >> 8) & 0xFF; 
      memcpy(&(pgm->lst.regs[reg]),&(pgm->pgm[pgm->pgm_count-2]),8);
      break;

    case TOK_JSR:
      n = (int32_t)(pgm->pgm[pgm->cur_ln++]);
       
      reg = n & 0xFF;
      dbgtrc("arr: %08X",n);
      if ((n & 0xFF00) != 0xFF00)
        v = pgm->lst.regs[(n >> 8) & 0xFF];
      else v = valnil;

      dbgtrc("%d %d %lX",pgm->cur_ln,reg,v);
        
      start_JSR(pgm, reg, v);
 
      dbgtrc("pgm stack: -1 %08X\n                 -2 %08X\n                 -3 %08X",pgm->pgm[pgm->pgm_count-1],pgm->pgm[pgm->pgm_count-2],pgm->pgm[pgm->pgm_count-3]);
      pgm->cur_ln = op>>8;
      
      break;

    case TOK_ZSR: {
//        int32_t dest;
//       _dbgtrc("BSR: op: %08X arg: %d %lX", op, pgm->arg_reg, pgm->lst.regs[pgm->arg_reg]);
//        start_JSR(pgm, pgm->arg_reg, pgm->lst.regs[pgm->arg_reg]);
//       _dbgtrc("pgm stack: -1 %08X\n                 -2 %08X\n                 -3 %08X",pgm->pgm[pgm->pgm_count-1],pgm->pgm[pgm->pgm_count-2],pgm->pgm[pgm->pgm_count-3]);
//        dest = pgm->lst.regs[(op>>8) & 0XFF];
//        if (dest > pgm->max_pgm) dest = pgm->max_pgm;
//        pgm->cur_ln = dest ;
      }
      break;

    case TOK_RET:
     _dbgtrc("ret stack: -1 %08X\n                 -2 %08X\n                 -3 %08X",pgm->pgm[pgm->pgm_count-1],pgm->pgm[pgm->pgm_count-2],pgm->pgm[pgm->pgm_count-3]);
      if (pgm->pgm_count > pgm->max_pgm+1) {
        reg = pgm->pgm[pgm->pgm_count-3] & 0xFF;
        if (reg != 0xFF) {
          memcpy(&(pgm->lst.regs[reg]),&(pgm->pgm[pgm->pgm_count-2]),8);
        }
        pgm->cur_ln = pgm->pgm[pgm->pgm_count-3] >> 8;
        pgm->pgm_count -= 3;
      }
     _dbgtrc("RET reg: %d %lX",reg,pgm->lst.regs[reg]);
      break;

    case TOK_RTV:
      reg = (op >> 8) & 0xFF;
     _dbgtrc("RTV: %d %lX",reg,pgm->lst.regs[reg]);
      memcpy(&(pgm->pgm[pgm->pgm_count-2]), &(pgm->lst.regs[reg]),8);
      break;

    case TOK_RT2: 
      v = val(((int32_t)op)>>16);
      memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      break;

    case TOK_RT4:
      v = val((int32_t)(pgm->pgm[pgm->cur_ln++]));
      memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      break;

    case TOK_RT8:
      memcpy( &v, &(pgm->pgm[pgm->cur_ln]), 8);
      v = const2str(pgm->str,v);
      memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      pgm->cur_ln+=2;
      break;

    case TOK_RTI:
      n = valtoint(pgm->lst.regs[(op & 0x00FF0000) >> 16]);
      memcpy( &v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
      v = const2str(pgm->str,v);
      memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      pgm->cur_ln++;
      break;

    case TOK_RTN:
      n = op >> 16;
      memcpy( &v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
      v = const2str(pgm->str,v);
      memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      pgm->cur_ln++;
      break;

    case TOK_NOP:
      pgm->cur_ln += op>>8;
      break;

    case TOK_SAV:
      for (int k = 8; k<=24; k+=8) {
        reg = (op >> k) & 0xFF;
        if (reg == 0xFF) break;
        if (pgm->lst_count >= pgm->lst_size) break;
        pgm->lst.regs[pgm->lst_count++] = pgm->lst.regs[reg];
      }
      break;

    case TOK_RCL:
      for (int k = 8; k<=24; k+=8) {
        reg = (op >> k) & 0xFF;
        if (reg == 0xFF) break;
        if (pgm->lst_count <= pgm->max_regs+1) break;
        pgm->lst.regs[reg] = pgm->lst.regs[--pgm->lst_count];
      }
      break;
 
     case 0x80 | TOK_SYS:
       p = &(pgm->pgm[pgm->cur_ln]);
       memcpy(&fname,p,8);
       fptr = kaj_checkfname(fname);
      _dbgtrc("SYS: f: %lX (%s) -> %p",fname, kaj_sys_decode(fname), (void *)fptr);
       if (fptr == NULL) { fprintf(stderr, "Unknown function: %s\n",kaj_sys_decode(fname)); abort();}
       pgm->pgm[pgm->cur_ln-1] &= 0xFFFFFF7F;
       memcpy(p,&fptr,8);

    case TOK_SYS:
       p = &(pgm->pgm[pgm->cur_ln]);
       memcpy(&fptr,p,8);

       v = valnil;
       if ((pgm->pgm[pgm->cur_ln-1] & 0xFF0000) != 0xFF0000) 
         v = pgm->lst.regs[(pgm->pgm[pgm->cur_ln-1] & 0xFF0000) >> 16];
        
       r = fptr(v);
       if ((pgm->pgm[pgm->cur_ln-1] & 0xFF00) != 0xFF00) 
         pgm->lst.regs[(pgm->pgm[pgm->cur_ln-1] & 0xFF00) >> 8] = r;
        
       pgm->cur_ln += 2;
       break;

    case TOK_VEC:
      reg = (op >> 8) & 0xFF; 
      pgm->lst.regs[reg] = valvec(((uint32_t)op) >> 16);
      break;

    case TOK_BUF:
      reg = (op >> 8) & 0xFF; 
      pgm->lst.regs[reg] = valbuf(((uint32_t)op) >> 16);
      break;

    case TOK_KLL:
      reg = (op >> 8) & 0xFF; 
      valfree(pgm->lst.regs[reg]);
      pgm->lst.regs[reg] = valnil;
      break;

    case TOK_CLR:
      reg = (op >> 8) & 0xFF; 
      valclear(pgm->lst.regs[reg]);
      break;

    case TOK_SET:
      valset(pgm->lst.regs[(op >> 8) & 0xFF],pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      break;

    case TOK_GET:
      v = valget(pgm->lst.regs[(op >> 16) & 0xFF],pgm->lst.regs[(op >> 24) & 0xFF]);
      pgm->lst.regs[(op >> 8) & 0xFF] = v;
      break;

    case TOK_LEN:
      reg = (op >> 8) & 0xFF; 
      n = valcount(pgm->lst.regs[(op >> 16) & 0xFF]);
      dbgtrc("LEN: %d",n);
      pgm->lst.regs[reg] = val(n);
      break;

    case TOK_SZE:
      reg = (op >> 8) & 0xFF; 
      pgm->lst.regs[reg] = val(valsize(pgm->lst.regs[(op >> 16) & 0xFF]));
      break;

    case TOK_PSH:
      valpush(pgm->lst.regs[(op >> 8) & 0xFF], pgm->lst.regs[(op >> 16) & 0xFF]);
      break;

    case TOK_T0P:
      reg = (op >> 8) & 0xFF; 
      pgm->lst.regs[reg] = valtop(pgm->lst.regs[(op >> 16) & 0xFF], op>>24);
      break;

    case TOK_TOP:
      reg = (op >> 8) & 0xFF; 
     _dbgtrc("TOPXX: %d %lX", valtoint(pgm->lst.regs[(op >> 24) & 0xFF]),pgm->lst.regs[(op >> 24) & 0xFF]);
      pgm->lst.regs[reg] = valtop(pgm->lst.regs[(op >> 16) & 0xFF], valtoint(pgm->lst.regs[(op >> 24) & 0xFF]));
      break;

    case TOK_DRP:
      valdrop(pgm->lst.regs[(op >> 8) & 0xFF], (op >> 16) & 0xFF);
      break;

    case TOK_ENQ:
      valenq(pgm->lst.regs[(op >> 8) & 0xFF], pgm->lst.regs[(op >> 16) & 0xFF]);
      break;

    case TOK_N0Q:
      reg = (op >> 8) & 0xFF; 
      n= op>>24;
      if (n>0) n--; // because valdeq starts from 0 not 1
      pgm->lst.regs[reg] = valhead(pgm->lst.regs[(op >> 16) & 0xFF], n);
      break;

    case TOK_NXQ:
      reg = (op >> 8) & 0xFF; 
      n = valtoint(pgm->lst.regs[(op >> 24) & 0xFF]);
      if (n>0) n--; // because valdeq starts from 0 not 1
     _dbgtrc("NXTXX: %d %lX", valtoint(pgm->lst.regs[(op >> 24) & 0xFF]),pgm->lst.regs[(op >> 24) & 0xFF]);
      pgm->lst.regs[reg] = valhead(pgm->lst.regs[(op >> 16) & 0xFF], n);
      break;

    case TOK_DEQ:
      valdeq(pgm->lst.regs[(op >> 8) & 0xFF], (op >> 16) & 0xFF);
      break;

    case TOK_STR:
      reg = (op >> 8) & 0xFF; 
      n = pgm->pgm[pgm->cur_ln++];
      if (n < pgm->str_count) pgm->lst.regs[reg] = val(pgm->str + n);
      else pgm->lst.regs[reg] = valnilstr;
      break;

    case TOK_S7R:
      reg = (op >> 8) & 0xFF; 
      n = pgm->pgm[pgm->cur_ln++];
      if (n < pgm->str_count) pgm->lst.regs[reg] = val(pgm->str + n);
      else pgm->lst.regs[reg] = valnilstr;
      break;

    default: pgm->cur_ln += (kaj_opcode_len(op)-1);

  }
  return 1;
}

int kaj_run(kaj_pgm_t pgm, int32_t start)
{

  if (pgm->lst_type != LST_REGISTERS) return ERR_NOTASSEMBLED_PGM;
  
  if (start >=0) pgm->cur_ln = start;

  while(kaj_step(pgm));

  return 0;
}
