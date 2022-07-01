//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#ifndef KANJI_H_
#define KANJI_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "dbg.h"
#include "skp.h"
#include "try.h"
#include "val.h"
#include "vrg.h"

#include "opcodes_tok.h"
extern char *opcodes;


/************************************/

#define LST_UNSORTED_LBL 0
#define LST_SORTED_LBL   1
#define LST_REGISTERS    2

#define FLG_LESSER      0x01
#define FLG_EQUAL       0x02
#define FLG_GREATER     0x04
#define FLG_DATA        0x10
#define FLG_STATICSTR   0x20

#define STR_OFFSET       1

typedef struct kaj_pgm_s {
  uint32_t *pgm;
  union {
    uint64_t *lbl;
    val_t    *regs;
  } lst;
  val_t     vecs;
  val_t     bufs;
  char     *str; // constant strings
  int32_t   str_size;
  int32_t   str_count;
  int32_t   lst_size;
  int32_t   lst_count;
  int32_t   pgm_size;
  int32_t   pgm_count;
  int32_t   max_pgm;
  int32_t   cur_ln;
  int32_t   cur_col;
  int16_t   pgm_err;
  uint32_t  dta_start;
  uint16_t  stk_size;
  uint8_t   pgm_flg;
  uint8_t   pgm_cmpflg;
  uint8_t   lst_type;
  uint8_t   max_regs;
} *kaj_pgm_t;

#define LBL_INVALID 0xFFFFFFFF

#define ERR_NONE              0
#define ERR_SYNTAX            1
#define ERR_INVALID_LBL       2
#define ERR_INVALID_OPCODE    3
#define ERR_INVALID_REG       4
#define ERR_INVALID_ARG       5
#define ERR_NO_MEMORY         6
#define ERR_UNDEFINED_LBL     7
#define ERR_DUPLICATE_LBL     8
#define ERR_EXECUTABLE_PGM    9
#define ERR_NOTASSEMBLED_PGM 10
#define ERR_DATASECTION      11

#define REG_NONE           0xFF

extern char *ERR_MSG[];

      void *kaj_free(kaj_pgm_t pgm);
       void kaj_dump(kaj_pgm_t pgm, FILE *f);
kaj_pgm_t kaj_new(int32_t pgm_init_size, int32_t lbl_init_size, uint16_t stk_size);

    #define kaj_addline(...) vrg(kaj_add_line,__VA_ARGS__)
    #define kaj_add_line2(p,l) kaj_add_line3(p, l, NULL)
    int32_t kaj_add_line3(kaj_pgm_t pgm, char *line, char **lnend);

        int kaj_assemble(kaj_pgm_t pgm);
        int kaj_fromfile(kaj_pgm_t pgm, FILE *f, FILE *errfile);
        int kaj_fromstring(kaj_pgm_t pgm, char *lines, FILE *errfile);

       void kaj_perror(kaj_pgm_t pgm,char *line, FILE *errfile);

        int kaj_opcode_len(uint32_t op);

        int kaj_run(kaj_pgm_t pgm, int32_t start);
        int kaj_step(kaj_pgm_t pgm);
        int kaj_init(kaj_pgm_t pgm, int32_t start, uint8_t reg, val_t input);

      val_t kaj_getreg(kaj_pgm_t pgm, uint8_t reg);
        int kaj_setreg(kaj_pgm_t pgm, uint8_t reg, val_t input);

      val_t xpr_eval(kaj_pgm_t pgm,char *xpr);

#endif
