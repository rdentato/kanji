#include "kanji.h"

#define VAL_MAIN
#include "val.h"

#undef  TOK_HLT
#define TOK_HLT 0x00

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

static uint32_t pop_pgm(kaj_pgm_t pgm)
{
  if (pgm->pgm_count <= pgm->max_pgm+1) return 0;
  return pgm->pgm[--pgm->pgm_count];
}

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

  pgm->pgm_count = pgm->max_pgm+1;  // reset pgm stack
  pgm->lst_count = pgm->max_regs+1; // reset regs stack
  pgm->cur_ln = start;
 _dbgtrc("Input: %lX",input);
  start_JSR(pgm,0,input);

  pgm->pgm[pgm->pgm_count-3] &= 0xFF;
  pgm->pgm[pgm->pgm_count-3] |= pgm->max_pgm << 8;

  pgm->pgm_flg = 0;

  return ERR_NONE;
}

int kaj_step(kaj_pgm_t pgm) 
{
  uint32_t op;
  uint8_t reg;

  op = pgm->pgm[pgm->cur_ln++];
  dbgtrc("STEP: %03X %08X",pgm->cur_ln-1,op);
  switch(op & 0x7F) {

    case TOK_HLT: return 0;

    case TOK_STO:
      pgm->lst.regs[(op >> 8) & 0xFF] = pgm->lst.regs[(op >> 16) & 0xFF];
      break;

    case TOK_ST2: {
        int32_t n;
        n = ((int32_t)op)>>16;
        pgm->lst.regs[(op >> 8) & 0xFF] = val(n);
       _dbgtrc("n2: %d -> %d",n,valtoint(pgm->lst.regs[(op >> 8) & 0xFF]));
      }
      break;

    case TOK_ST4: {
        int32_t n = (int32_t)(pgm->pgm[pgm->cur_ln++]);
        pgm->lst.regs[(op >> 8) & 0xFF] = val(n);
       _dbgtrc("n: %d -> %d",n,valtoint(pgm->lst.regs[(op >> 8) & 0xFF]));
      }
      break;

    case TOK_ST8:
      { void *p;
        p = &(pgm->lst.regs[(op >> 8) & 0xFF]);
        memcpy(p, &(pgm->pgm[pgm->cur_ln]), 8);
        pgm->cur_ln+=2;
      }
      break;

    case TOK_STI:
      { void *p;
        p = &(pgm->lst.regs[(op >> 8) & 0xFF]);
        int32_t n;
        n = valtoint(pgm->lst.regs[(op & 0x00FF0000) >> 16]);
        memcpy(p, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
        pgm->cur_ln++;
      }
      break;

    case TOK_STN: {
        void *p;
        p = &(pgm->lst.regs[(op >> 8) & 0xFF]);
        int32_t n = op >> 16;
        memcpy(p, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
        pgm->cur_ln++;
      }
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

    case TOK_INT: {
        val_t v;
        v = pgm->lst.regs[(op >> 16) & 0xFF];
        if (valisdbl(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val((int32_t)valtodbl(v));
          break;
        }
        if (valisint(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = v;
          break;
        }
      }
      break;

    case TOK_FLT: {
        val_t v;
        v = pgm->lst.regs[(op >> 16) & 0xFF];
        if (valisdbl(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = v;
          break;
        }
        if (valisint(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val((double)valtoint(v));
          break;
        }
      }
      break;

    case TOK_NOT: {
        val_t v;
        v = pgm->lst.regs[(op >> 16) & 0xFF];
        if (valisdbl(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(((int32_t)valtodbl(v)) ^ 0xFFFFFFFF);
          break;
        }
        if (valisint(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtoint(v) ^ 0xFFFFFFFF);
          break;
        }
      }
      break;

    case TOK_NEG: {
        val_t v;
        v = pgm->lst.regs[(op >> 16) & 0xFF];
        if (valisdbl(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtodbl(v) * -1.0);
          break;
        }
        if (valisint(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtoint(v) * -1) ;
          break;
        }
      }
      break;

    case TOK_INC: {
        val_t v;
        int32_t inc;
        v = pgm->lst.regs[(op >> 8) & 0xFF];
        inc = op >> 16;
       _dbgtrc("inc: %d",inc);
        if (valisdbl(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtodbl(v) + inc);
          break;
        }
        if (valisint(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtoint(v) + inc) ;
          break;
        }
      }
      break;

    case TOK_DEC: {
        val_t v;
        int32_t dec;
        v = pgm->lst.regs[(op >> 8) & 0xFF];
        dec = op >> 16;
        if (valisdbl(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtodbl(v) - dec);
          break;
        }
        if (valisint(v)) {
          pgm->lst.regs[(op >> 8) & 0xFF] = val(valtoint(v) - dec) ;
          break;
        }
      }
      break;

    case TOK_CMP: {
        int n;
        n = valcmp(pgm->lst.regs[(op >> 8) & 0xFF], pgm->lst.regs[(op >> 16) & 0xFF]);
        pgm->pgm_flg = "\1\2\4"[n+1];
      }
      break;

    case TOK_CP2: {
        int n;
        n = valcmp(pgm->lst.regs[(op >> 8) & 0xFF], val(((int32_t)op) >> 16));
        pgm->pgm_flg = "\1\2\4"[n+1];
      }
      break;

    case TOK_CP4: {
        int n;
        n = valcmp(pgm->lst.regs[(op >> 8) & 0xFF], val((int32_t)(pgm->pgm[pgm->cur_ln++])));
        pgm->pgm_flg = "\1\2\4"[n+1];
      }
      break;

    case TOK_CP8:
      {
        int n;
        val_t v;
        memcpy(&v, &(pgm->pgm[pgm->cur_ln]), 8);
        n = valcmp( pgm->lst.regs[(op >> 8) & 0xFF], v);
        pgm->pgm_flg = "\1\2\4"[n+1];
        pgm->cur_ln+=2;
      }
      break;

    case TOK_CPI:
      { val_t v;
        int32_t n;
        n = valtoint(pgm->lst.regs[(op & 0x00FF0000) >> 16]);
        memcpy(&v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
       _dbgtrc("R: %lX, V: %lX",pgm->lst.regs[(op >> 8) & 0xFF],*(uint64_t *)(pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2)));
        n = valcmp( pgm->lst.regs[(op >> 8) & 0xFF], v);
        pgm->pgm_flg = "\1\2\4"[n+1];
        pgm->cur_ln++;
      }
      break;

    case TOK_CPN:
      { val_t v;
        int32_t n;
        n = op >> 16;
        memcpy(&v, pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
       _dbgtrc("R: %lX, V: %lX",pgm->lst.regs[(op >> 8) & 0xFF],*(uint64_t *)(pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2)));
        n = valcmp( pgm->lst.regs[(op >> 8) & 0xFF], v);
        pgm->pgm_flg = "\1\2\4"[n+1];
        pgm->cur_ln++;
      }
      break;

    case TOK_DTA:
    case TOK_JMP: 
      pgm->cur_ln = op >> 8;
      break;
 
    case TOK_JEQ: 
      if (pgm->pgm_flg & FLG_EQUAL) pgm->cur_ln = op >> 8;
      break;
 
    case TOK_JNE: 
      if (!(pgm->pgm_flg & FLG_EQUAL)) pgm->cur_ln = op >> 8;
      break;
 
    case TOK_JGT: 
      if (pgm->pgm_flg & FLG_GREATER) pgm->cur_ln = op >> 8;
      break;

    case TOK_JGE: 
      if (pgm->pgm_flg & (FLG_EQUAL | FLG_GREATER)) pgm->cur_ln = op >> 8;
      break;

    case TOK_JLT: 
      if (pgm->pgm_flg & FLG_LESSER) pgm->cur_ln = op >> 8;
      break;

    case TOK_JLE: 
      if (pgm->pgm_flg & (FLG_EQUAL | FLG_LESSER)) pgm->cur_ln = op >> 8;
      break;

    case TOK_BLE:
      if (pgm->pgm_flg & (FLG_EQUAL | FLG_LESSER)) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_BMP: 
      pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;
 
    case TOK_BEQ: 
      if (pgm->pgm_flg & FLG_EQUAL) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];;
      break;
 
    case TOK_BNE: 
      if (!(pgm->pgm_flg & FLG_EQUAL)) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;
 
    case TOK_BGT: 
      if (pgm->pgm_flg & FLG_GREATER) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_BGE: 
      if (pgm->pgm_flg & (FLG_EQUAL | FLG_GREATER)) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_BLT: 
      if (pgm->pgm_flg & FLG_LESSER) pgm->cur_ln = pgm->lst.regs[(op >> 8) & 0xFF];
      break;

    case TOK_ARR:
      reg = (op >> 8) & 0xFF;
      if (reg != 0xFF) pgm->arg_reg = reg;
      break;

    case TOK_ARG:
      reg = (op >> 8) & 0xFF; 
      memcpy(&(pgm->lst.regs[reg]),&(pgm->pgm[pgm->pgm_count-2]),8);
      break;

    case TOK_JSR:
     _dbgtrc("JSR: op: %08X arg: %d %lX", op, pgm->arg_reg, pgm->lst.regs[pgm->arg_reg]);
      start_JSR(pgm, pgm->arg_reg, pgm->lst.regs[pgm->arg_reg]);
     _dbgtrc("pgm stack: -1 %08X\n                 -2 %08X\n                 -3 %08X",pgm->pgm[pgm->pgm_count-1],pgm->pgm[pgm->pgm_count-2],pgm->pgm[pgm->pgm_count-3]);
      pgm->cur_ln = op>>8;
      break;

    case TOK_BSR: {
        int32_t dest;
       _dbgtrc("BSR: op: %08X arg: %d %lX", op, pgm->arg_reg, pgm->lst.regs[pgm->arg_reg]);
        start_JSR(pgm, pgm->arg_reg, pgm->lst.regs[pgm->arg_reg]);
       _dbgtrc("pgm stack: -1 %08X\n                 -2 %08X\n                 -3 %08X",pgm->pgm[pgm->pgm_count-1],pgm->pgm[pgm->pgm_count-2],pgm->pgm[pgm->pgm_count-3]);
        dest = pgm->lst.regs[(op>>8) & 0XFF];
        if (dest > pgm->max_pgm) dest = pgm->max_pgm;
        pgm->cur_ln = dest ;
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

    case TOK_RT2: {
        val_t v;
        v = val(((int32_t)op)>>16);
        memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      }
      break;

    case TOK_RT4: {
        val_t v;
        v = val((int32_t)(pgm->pgm[pgm->cur_ln++]));
        memcpy( &(pgm->pgm[pgm->pgm_count-2]), &v, 8);
      }
      break;

    case TOK_RT8:
      memcpy( &(pgm->pgm[pgm->pgm_count-2]), &(pgm->pgm[pgm->cur_ln]), 8);
      pgm->cur_ln+=2;
      break;

    case TOK_RTI: {
        int32_t n = 0;
        n = valtoint(pgm->lst.regs[(op & 0x00FF0000) >> 16]);
        memcpy( &(pgm->pgm[pgm->pgm_count-2]), pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
        pgm->cur_ln++;
      }
      break;

    case TOK_RTN: {
        int32_t n = op >> 16;
        memcpy( &(pgm->pgm[pgm->pgm_count-2]), pgm->pgm + pgm->pgm[pgm->cur_ln] + (n*2), 8);
        pgm->cur_ln++;
      }
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
