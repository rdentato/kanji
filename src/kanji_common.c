#define SKP_MAIN
#define TRY_MAIN
#define VAL_MAIN
#include "kanji.h"

#include "opcodes_str.h"

int kaj_opcode_len(uint32_t op)
{
  int len = 1;
  switch(op & 0x7F) {
    case TOK_SYS: 
    case TOK_RT8: 
    case TOK_CP8: 
    case TOK_ST8: len++;
    
    case TOK_RTN:
    case TOK_CPN:
    case TOK_STN:
    case TOK_RTI:
    case TOK_CPI:
    case TOK_STI:
    case TOK_RT4:
    case TOK_CP4:
    case TOK_ST4: 
    case TOK_JSR: len++;
                  break;

    case TOK_NOP: len += op >> 8;
                  break;

  }
  _dbgtrc("LEN: %02X %d",op&0x7F,len);
  return len;
}


kaj_pgm_t kaj_new(int32_t pgm_init_size, int32_t lbl_init_size, uint16_t stk_size)
{
  kaj_pgm_t pgm;

  pgm = malloc(sizeof(struct kaj_pgm_s));
  assert(pgm);

  if (pgm_init_size <= 0) pgm_init_size = 1024;
  if (lbl_init_size <= 0) lbl_init_size = 256;

  pgm->pgm = malloc(sizeof(uint32_t) * pgm_init_size);
  assert(pgm->pgm);

  pgm->lst.lbl = malloc(sizeof(uint64_t) * lbl_init_size);
  assert(pgm->lst.lbl);

  pgm->lst_size  = lbl_init_size;
  pgm->lst_count = 0;
  pgm->lst_type  = LST_UNSORTED_LBL;

  pgm->pgm_size  = pgm_init_size;
  pgm->pgm_count = 0; 
  pgm->pgm_err   = 0;
  pgm->stk_size  = 256;
  pgm->max_regs  = 0;
  pgm->pgm_flg   = 0;
  pgm->cur_ln = 0;

  return pgm;
}

void *kaj_free(kaj_pgm_t pgm)
{
  if (pgm) {
    pgm->lst_size  = 0;
    pgm->lst_count = 0;
    pgm->pgm_size  = 0;
    pgm->pgm_count = 0; 
    if (pgm->lst.lbl) free (pgm->lst.lbl);
    if (pgm->pgm) free (pgm->pgm);
    free(pgm);
  }
  return NULL;
}

void kaj_dump(kaj_pgm_t pgm, FILE *f)
{
  uint8_t *bytes;
  for (int32_t k=0; k<pgm->pgm_count; k++) {
    bytes = (uint8_t *)(pgm->pgm + k);
    //printf("%06X  %02X %02X %02X %02X\n", k, bytes[3], bytes[2], bytes[1], bytes[0]);
    fprintf(stderr,"%06X  %02X %02X %02X %02X\n", k, bytes[0], bytes[1], bytes[2], bytes[3]);
  }
}
