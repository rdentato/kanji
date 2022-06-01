#include "kanji.h"
#include "kanji_sys.h"

char *ERR_MSG[] = {
  "",
  "Syntax Error",
  "Invalid label",
  "Invalid opcode",
  "Invalid register",
  "Invalid or missing argument",
  "Out of memory",
  "Undefined label",
  "Duplicate label",
  "Executable program",
  "Unassembled program",
  "Code in data section",
  NULL
};

static uint32_t lbl2int(char *s)
{
  uint32_t lblint = LBL_INVALID;
  if (*s == '@') lblint = strtol(s+1,NULL,16);
  if (lblint & 0xFF000000) lblint = LBL_INVALID;
  return lblint;
}

static void add_label(kaj_pgm_t pgm, char *lbl)
{
  uint32_t lblint;
  throwif (pgm->lst_type == LST_REGISTERS, ERR_EXECUTABLE_PGM);
  if (pgm->lst_count >= pgm->lst_size ) {
    pgm->lst_size += (pgm->lst_size / 2);
    pgm->lst.lbl = realloc(pgm->lst.lbl, sizeof(uint64_t) * pgm->lst_size);
    throwif(pgm->lst.lbl == NULL, ERR_NO_MEMORY);
  }
  lblint = lbl2int(lbl);
  throwif(lblint == LBL_INVALID,  ERR_INVALID_LBL);
  pgm->lst.lbl[pgm->lst_count++] = ((uint64_t)lblint) << 32 | pgm->pgm_count;
}

static uint32_t add_reg(kaj_pgm_t pgm, char *s)
{
  uint32_t reg;
  reg = strtol(s+1,NULL,16);
  if ((reg != 0xFF) && (pgm->max_regs < reg))
    pgm->max_regs = reg;
  return reg;
}

static void pgm_makeroom(kaj_pgm_t pgm, int32_t n)
{
  int32_t sz = pgm->pgm_size;

  while (sz <= pgm->pgm_count+n)
    sz += sz/2;
  
  if ( sz > pgm->pgm_size) {
    pgm->pgm_size = sz;
    pgm->pgm = realloc(pgm->pgm,sizeof(uint32_t) * pgm->pgm_size);
    throwif(pgm->pgm == NULL, ERR_NO_MEMORY);
  }
}

static void str_makeroom(kaj_pgm_t pgm, int32_t n)
{
  int32_t sz = pgm->str_size;
  if (sz == 0) sz = 8;

  while (sz <= pgm->str_count+n)
    sz += sz/2;
  
  if ( sz > pgm->str_size) {
    pgm->str_size = sz;
    pgm->str = realloc(pgm->str, pgm->str_size);
    throwif(pgm->str == NULL, ERR_NO_MEMORY);
  }
}

static void add_long(kaj_pgm_t pgm, uint32_t n)
{
  pgm_makeroom(pgm,1);
  pgm->pgm[pgm->pgm_count++] = n;
}

static void add_double(kaj_pgm_t pgm,double d)
{
  pgm_makeroom(pgm,2);
  memcpy(pgm->pgm + pgm->pgm_count, &d, 8);
  pgm->pgm_count += 2;
}

static void add_val(kaj_pgm_t pgm,val_t v)
{
  pgm_makeroom(pgm,2);
  memcpy(pgm->pgm + pgm->pgm_count, &v, 8);
  pgm->pgm_count += 2;
}

static char escaped_char(char **start)
{
  char *s = *start;
  int c = 0;
  c = *s++;
  if ( c == '\\') {
    c = 0;
    if ('0' <= *s && *s <= '7') {
      for (int k = 0; k<3 ; k++) {
        c = (c<<3) | (*s - '0');
        s++;
        if ( *s < '0' || '7' < *s) break;
      }
    }
    else if (*s == 'x') {
      s++;
      for (int k = 0; k<2 ; k++) {
        if ('0' <= *s && *s <= '9') 
          c = (c << 4) | (*s - '0');
        else if (('A' <= *s && *s <= 'F') || ('a' <= *s && *s <= 'f'))
          c = (c << 4) | ((*s - 1) & 0x0F);
        else break;
        s++;
      }
    }
    else {
      switch (*s) {
        case 'n' : c = '\n'; break;
        case 'r' : c = '\r'; break;
        case 'f' : c = '\f'; break;
        case 'b' : c = '\b'; break;
        default  : c = *s ; break;
      }
      s++;
    }
  }
  *start = s;
  return c;
}

static void add_str(kaj_pgm_t pgm, char *s, int32_t n)
{
  val_t v;

 _dbgtrc("ADDSTR: [%.*s] @ %d",n,s+1,pgm->str_count);

  v = valconst(STR_OFFSET,pgm->str_count);
  str_makeroom(pgm,n+1);

  s++;
  char c;
  while (*s != '"') {
    c = escaped_char(&s);
    pgm->str[pgm->str_count++] = c;
  }
  pgm->str[pgm->str_count++] = '\0';
  add_val(pgm,v);
}

static uint32_t check_opcode(kaj_pgm_t pgm, char *op)
{
   char candidate_op[4];
   candidate_op[0] = toupper(op[0]);
   candidate_op[1] = toupper(op[1]);
   candidate_op[2] = toupper(op[2]);
   candidate_op[3] = '\0';

   int32_t i,j,m;
   int c;

   i = 0; 
   j = NUMOPCODES-1;
   while (i<=j) {
     m = (i+j)/2;
     c = strncmp(candidate_op, opcodes + m*4, 3);
     // skptrace("OPC check '%.3s' gives %d",opcodes+m*3,c);
     if (c == 0) return m+1;
     if (c < 0) j = m-1;
     else i = m+1;
   }
   throw(ERR_INVALID_OPCODE);
   return 0;
}

static char *arg_S_C_R(kaj_pgm_t pgm, uint32_t op, char *s, char *start, uint8_t op2, uint8_t op4, uint8_t op8, uint8_t opI, uint8_t opN)
{
  char *t=s;
  uint32_t reg;
  int32_t sh = 16;

  if ((op & 0xFF) == TOK_RTV) sh = 8;

  if (skp(s,"!C 'NIL' W",&t)) {
    val_t v = valnil;
    op = (op & 0xFF00) | op8;
    add_long(pgm,op);
    add_val(pgm,v);
    return t;
  }

  if (skp(s,"!C 'TRUE' W",&t)) {
    val_t v = valtrue;
    op = (op & 0xFF00) | op8;
    add_long(pgm,op);
    add_val(pgm,v);
    return t;
  }

  if (skp(s,"!C 'FALSE' W",&t)) {
    val_t v = valfalse;
    op = (op & 0xFF00) | op8;
    add_long(pgm,op);
    add_val(pgm,v);
    return t;
  }

  if ((*s == '"') && skp(s,"Q",&t)) {
    op = (op & 0xFF00) | op8;
    add_long(pgm,op);
    add_str(pgm,s,(int32_t)(t-s)-2);
    skp(t,"W",&t);
    return t;
  }

  // STO R[X] R[Y]
  if (skp(s,"'%'x?x W",&t)) {
    reg = add_reg(pgm,s);
    op |= reg << sh;
    add_long(pgm,op);
    return t;
  }

  // STO R[X] 3.4
  if (skp(s,"D '.' *d W",&t)) {
    double d = strtod(s,NULL);
    op = (op & 0xFF00) | op8;
    add_long(pgm,op);
    add_double(pgm,d);
    return t;
  }

  int32_t n=0;

  if (skp(s,"'@'X W",&t)) {
    n = strtol(s+1,NULL,16);
    throwif((uint32_t)n > 0x00FFFFFF,ERR_INVALID_LBL,(int16_t)(t-start));

    s = t;
    op &= 0xFF00; // keep register only

    if (skp(s,"'[' W ",&t)) {
      int32_t k = -1;
      s = t;
      if (skp(s,"'$' X W ']' W",&t)) {
        k = strtol(s+1,NULL,16);
      }
      else if (skp(s,"D W ']' W",&t)) {
        k = strtol(s,NULL,10);
      }
      dbgtrc("IND: k=%d s= %s",k,s);
      if (k >= 0) {
        op |= ((k & 0xFFFF) << 16) | opN;
      }
      else if (skp(s,"'%'x?x W ']' W",&t)) {
        reg = add_reg(pgm,s);
        op = reg << sh | (op & 0xFF00) | opI;
      }
      else throw(ERR_INVALID_ARG,(int16_t)(t-start));
    }
    else op |= op4;

    add_long(pgm,op | 0x80);
    add_long(pgm,n);
    return t;    
  }

  if (skp(s,"'$' X W",&t)) {
    n = strtol(s+1,NULL,16);
  }
  else if (skp(s,"D W",&t)) {
    n = strtol(s,NULL,10);
  }
  else if ((*s == '\'') && skp(s,"Q W",&t)) {
    s++;
    n = escaped_char(&s); 
  }
  else throw(ERR_INVALID_ARG,(int16_t)(t-start));

  if (n <= -32768 || 32767 < n) {
    op = (op & 0xFF00) | op4;
    add_long(pgm,op);
    add_long(pgm,n);
  }
  else {
    op = (n << 16) | (op & 0xFF00) | op2;
    add_long(pgm,op);
  }

  return t;
}

static char *arg_STO_CMP(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t reg;

  uint8_t op2 = TOK_ST2;
  uint8_t op4 = TOK_ST4;
  uint8_t op8 = TOK_ST8;
  uint8_t opI = TOK_STI;
  uint8_t opN = TOK_STN;
  
  if (op == TOK_CMP) {
    op2 = TOK_CP2;
    op4 = TOK_CP4;
    op8 = TOK_CP8;
    opI = TOK_CPI;
    opN = TOK_CPN;
  }

  if (!skp(s,"'%'x?x W",&t)) throw(ERR_INVALID_REG,0);
  
  reg = add_reg(pgm,s);
  op |= reg << 8;
  s = t;

  return arg_S_C_R(pgm,op,s,start,op2,op4,op8,opI,opN);
}

static char *arg_JMP(kaj_pgm_t pgm, uint32_t op, char *start)
{
  // JMP %02
  char *s = start;
  char *t = s;
  uint32_t reg;
  if (skp(s, "'%'x?x W", &t)) {
    reg = add_reg(pgm,s);
    op = op + (TOK_ZEQ - TOK_JEQ);
    op |= reg << 8;
    add_long(pgm,op);
    return t;
  }

  // JMP 421
  
  uint32_t dest = 0xFFFFFFFF;
  if (skp(s, "D W", &t)) {
    dest = strtol(s,NULL,10);
  }
  else if (skp(s, "[$@]X W", &t)) {
    dest = strtol(s+1,NULL,16);
  }

  if (dest <= 0x00FFFFFF) {
    if (*s == '@') op |= 0x80;
    op  |= (dest << 8);
    add_long(pgm,op);
    return t;
  }

  throw(ERR_INVALID_LBL,0);

  return t;
}

static char *arg_JSR(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t reg;
  uint32_t dest = 0xFFFFFFFF;

  if (skp(s, "'%'x?x W", &t)) {
    reg = add_reg(pgm,s);
    op = TOK_ZSR;
    op |= reg << 8;
  }
  else if (skp(s, "D W", &t)) {
    dest = strtol(s,NULL,10);
  }
  else if (skp(s, "[$@]X W", &t)) {
    dest = strtol(s+1,NULL,16);
  }
  else throw(ERR_INVALID_ARG,(int16_t)(t-start));

  if (dest <= 0x00FFFFFF) {
    if (*s == '@') op |= 0x80;
    op  |= (dest << 8);
  }
  s = t;

  if (skp(s, "'%'x?x W", &t)) {
    reg = add_reg(pgm,s);
    s = t;
  }
  else reg = 0xFF;

  if (skp(s, "'%'x?x W", &t)) {
    reg |= add_reg(pgm,s) << 8;
  }
  else reg |= (reg << 8);

  add_long(pgm,op);
  add_long(pgm, reg);

  return t;
}

static char *arg_SAV_RCL(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t reg1 = 0x0000FF00;
  uint32_t reg2 = 0x00FF0000;
  uint32_t reg3 = 0xFF000000;

  if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,0);

  reg1 = add_reg(pgm,s) << 8;
  s = t;

  if (skp(s, "'%'x?x W", &t)) {
    reg2 = add_reg(pgm,s) << 16;
    s = t;
 
    if (skp(s, "'%'x?x W", &t)) {
      reg3 = add_reg(pgm,s) << 24;
      s = t;
    }
  }
  add_long(pgm,op|reg1|reg2|reg3);
  return t;
}

static char *arg_n_regs(kaj_pgm_t pgm, uint32_t op, char *start, int n)
{
  char *s = start;
  char *t = s;
  uint32_t reg;

  if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,0);

  reg = add_reg(pgm,s);
  op |= reg << 8;
  s = t;

  if (n>1) {
    if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,(int16_t)(t-start));
  
    reg = add_reg(pgm,s);
    op |= reg << 16;
    s = t;
  
    if (n>2) {
      if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,(int16_t)(t-start));
    
      reg = add_reg(pgm,s);
      op |= reg << 24;
      s = t;
    }
  }
  add_long(pgm,op);
  return t;
}

static char *arg_3_regs(kaj_pgm_t pgm, uint32_t op, char *s)
{ return arg_n_regs(pgm, op, s, 3); }

static char *arg_2_regs(kaj_pgm_t pgm, uint32_t op, char *s)
{ return arg_n_regs(pgm, op, s, 2); }

static char *arg_1_regs(kaj_pgm_t pgm, uint32_t op, char *s)
{ return arg_n_regs(pgm, op, s, 1); }

static char *arg_reg_int(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t reg;
  int32_t n = -1;

  if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,0);

  reg = add_reg(pgm,s);
  op |= reg << 8;
  s = t;

  if (skp(s, "'$'X W", &t))    n = strtol(s,NULL,16);
  else if (skp(s, "D W", &t))  n = strtol(s,NULL,10);
  
  if (n <= 0) n = 1;
  else if (n > 0xFFFF) throw(ERR_INVALID_ARG,(int16_t)(t-start));

  op |= n << 16; 
  add_long(pgm,op);
  return t;
}

static char *arg_STR(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t reg;
  int32_t n = -1;

  if (skp(s, "'%'x?x W", &t)) {
    reg = add_reg(pgm,s);
    op |= reg << 8;
    s = t;
  }
  else throw(ERR_INVALID_ARG);

  if (skp(s, "'%'x?x W", &t)) {
    reg = add_reg(pgm,s);
    op |= reg << 16;
    add_long(pgm,op);
    return t;
  }

  if (skp(s, "'$'X W", &t))    n = strtol(s,NULL,16);
  else if (skp(s, "D W", &t))  n = strtol(s,NULL,10);
  
  if (n < 0) throw(ERR_INVALID_ARG,(int16_t)(t-start));

  op = (op & 0xFF00) | TOK_S7R; 
  add_long(pgm,op);
  add_long(pgm,n);
  return t;
}


static char *arg_SHIFT(kaj_pgm_t pgm, uint32_t op, char *start, uint32_t op_alt)
{
  char *s = start;
  char *t = s;
  uint32_t reg;
  uint32_t args = 0;;

  if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,0);

  reg = add_reg(pgm,s);
  args |= reg << 8;
  s = t;

  if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,(int16_t)(t-start));
  
  reg = add_reg(pgm,s);
  args |= reg << 16;
  s = t;
  
  if (skp(s, "'%'x?x W", &t)) {
    reg = add_reg(pgm,s);
    args |= (reg << 24);
    s = t;
  }
  else {
    int32_t n = -1;
    if (skp(s,"'$'x?x W",&t))  n = strtol(s+1,NULL,16);
    else if (skp(s,"+d W",&t))  n = strtol(s,NULL,10);
    if (n <= 0) n = 1;
    args |= (n & 0xFF) << 24;
    op = op_alt;
  }

  add_long(pgm, (args | op));
  return t;
}

static char *arg_SYS(kaj_pgm_t pgm, uint32_t op, char *start)
{
  // SYS m.f %01 %02
  char *s = start;
  char *t = s;
  uint64_t fnum = 0;
  uint8_t reg1 = 0xFF;
  uint8_t reg2 = 0xFF;
  
 _dbgtrc("f: %s",t);
  fnum = kaj_sys_encode(&t);
  throwif(fnum == 0,ERR_INVALID_ARG,0);

 _dbgtrc("f: %lX",fnum);

  s = t;
 _dbgtrc("1: %s",t);
  if (skp(s, "'%'x?x W", &t)) {
    reg1 = add_reg(pgm,s);
  }
  s = t;
  reg2 = reg1;   
 _dbgtrc("2: %s",t);
  if (skp(s, "'%'x?x W", &t)) {
    reg2 = add_reg(pgm,s);
  }

  // Setting the highest bit of TOK_SYS, signals that the
  // argument is the (encoded) name of the function.
  // the exec function will resolve the name into the
  // actual function.
  op  = (reg2 << 16)| (reg1 << 8) | (0x80 | TOK_SYS);
  add_long(pgm,op);

  void *p = pgm->pgm+pgm->pgm_count;
  add_long(pgm,0);
  add_long(pgm,0);
  memcpy(p,&fnum,8);
  return t;
}

static char *arg_NOP(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t n = 0;
  if (skp(s, "+d W", &t)) {
    n = strtol(s,NULL,10);
  }
  else if (skp(s, "$X W", &t)) {
    n = strtol(s,NULL,16);
  }
  
  if (n>0) n = (n+3)/4; // Size was in bytes
  add_long(pgm, (n << 8) | TOK_NOP);
  
  for (int k = 0; k< n; k++)
    add_long(pgm,0);

  return t;
}

static char *arg_VAL(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  int32_t n = 0;
  val_t v;

  if (!(pgm->pgm_flg & FLG_DATA)) {
    if ((pgm->lst_count > 0) && ((pgm->lst.lbl[pgm->lst_count-1] & 0xFFFFFFFF) == pgm->pgm_count)) {
      pgm->lst.lbl[pgm->lst_count-1] += 1;
    }
    pgm->pgm_flg |= FLG_DATA;
    pgm->dta_start = pgm->pgm_count;

    add_long(pgm,TOK_DTA);
    pgm->pgm_flg |= FLG_DATA;
  }

  if (skp(s,"!C 'NIL' W",&t)) { add_val(pgm,valnil); return t; }
  if (skp(s,"!C 'TRUE' W",&t)) { add_val(pgm,valtrue); return t; }
  if (skp(s,"!C 'FALSE' W",&t)) { add_val(pgm,valfalse); return t; }

  if (skp(s,"D '.' *d W",&t)) {
    v = val(strtod(s,NULL));
    add_val(pgm,v);
    return t;
  }

  if (*s == '"' && skp(s,"Q",&t)) {
    add_str(pgm, s, (int32_t)(t-s)-2);
    skp(t,"W",&t);
    return t; 
  }

  if (skp(s,"'$' X W",&t)) {
    n = strtol(s+1,NULL,16);
  }
  else if (skp(s,"D W",&t)) {
    n = strtol(s,NULL,10);
  }
  else if ((*s == '\'') && skp(s,"Q W",&t)) {
    s++;
    n = escaped_char(&s); 
  }
  else throw(ERR_INVALID_ARG,(int16_t)(t-start));

  v = val(n);
  add_val(pgm,v);
  return t;
}

static char *arg_VEC(kaj_pgm_t pgm, uint32_t op, char *start)
{
  char *s = start;
  char *t = s;
  uint32_t reg;

  if (!skp(s, "'%'x?x W", &t)) throw(ERR_INVALID_REG,0);

  reg = add_reg(pgm,s);
  op |= reg << 8;
  s = t;
 
  int32_t n = -1;

  if (skp(s,"'$'x?x W",&t))  n = strtol(s+1,NULL,16);
  else if (skp(s,"+d W",&t))  n = strtol(s,NULL,10);
  if (n <= 0) n = 16;
 
  add_long(pgm, ((n << 16) | op));
  return t;
}

int32_t kaj_addline(kaj_pgm_t pgm, char *line)
{
  char *s = line;
  char *t = s;

  pgm->cur_ln++;
  pgm->cur_col = 0;
  pgm->pgm_err = ERR_NONE;

  skp(s,"W",&s);

  // LABEL: OPC ARGS  
  try {
    if (skp(s, "'@' +x W ':' W", &t)) { // Label
      pgm->cur_col = (int32_t)(s-line);
      add_label(pgm,s);
      s = t;
    }
  
    if (skp(s, "aaa W", &t)) {
      uint32_t op;
      pgm->cur_col = (int32_t)(s-line);
      op =  check_opcode(pgm,s);
      //throwif((pgm->pgm_flg & FLG_DATA) && (op != TOK_VAL) && (op != TOK_HLT),ERR_DATASECTION);
      if ((pgm->pgm_flg & FLG_DATA) && (op != TOK_VAL)) {
        pgm->pgm[pgm->dta_start] |= pgm->pgm_count << 8;
        pgm->pgm_flg &= ~FLG_DATA;
      }

      switch (op) {

        case TOK_CMP : 
        case TOK_STO : 
          t = arg_STO_CMP(pgm,op,t);
          break;
        
        case TOK_JEQ :
        case TOK_JNE :
        case TOK_JGE :
        case TOK_JGT :
        case TOK_JLE :
        case TOK_JLT :
        case TOK_JMP :
          t = arg_JMP(pgm,op,t);
          break;

        case TOK_SYS :
          t = arg_SYS(pgm,op,t);
          break;

        case TOK_ADD :
        case TOK_SUB :
        case TOK_MUL :
        case TOK_DIV :
        case TOK_MOD :
        case TOK_AND :
        case TOK_ORR :
        case TOK_XOR :
        case TOK_SET :
        case TOK_GET :
          t = arg_3_regs(pgm,op,t);
          break;

        case TOK_SAV :
        case TOK_RCL :
          t = arg_SAV_RCL(pgm,op,t);
          break;

        case TOK_RTV :
          t = arg_S_C_R(pgm,op,t,t,TOK_RT2,TOK_RT4,TOK_RT8,TOK_RTI,TOK_RTN);
          break;

        case TOK_KLL :
        case TOK_CLR :
        case TOK_ARG :
          t = arg_1_regs(pgm,op,t);
          break;

        case TOK_PSH :
        case TOK_ENQ :
        case TOK_LEN :
        case TOK_SZE :
        case TOK_INT :
        case TOK_FLT :
        case TOK_NOT :
        case TOK_NEG :
          t = arg_2_regs(pgm,op,t);
          break;

        case TOK_JSR :
          t = arg_JSR(pgm,op,t);
          break;

        case TOK_DRP:
        case TOK_DEQ:
        case TOK_INC:
        case TOK_DEC:
          t = arg_reg_int(pgm,op,t);
          break;

        case TOK_STR:
          t = arg_STR(pgm,op,t);
          break;

        case TOK_NOP:
          t = arg_NOP(pgm,op,t);
          break;

        case TOK_HLT: pgm->pgm_flg &= ~FLG_DATA; op = 0;
        case TOK_RET:
          add_long(pgm,op);
          break;

        case TOK_DTA:
          pgm->pgm_flg |= FLG_DATA;
          pgm->dta_start = pgm->pgm_count;
          add_long(pgm,op);
          break;

        case TOK_ASR:
          t = arg_SHIFT(pgm,op,t,TOK_AR1);
          break;

        case TOK_SHL:
          t = arg_SHIFT(pgm,op,t,TOK_SL1);
          break;

        case TOK_SHR:
          t = arg_SHIFT(pgm,op,t,TOK_SR1);
          break;

        case TOK_VAL:
          t = arg_VAL(pgm,op,t);
          break;

        case TOK_BUF:
        case TOK_VEC:
          t = arg_VEC(pgm,op,t);
          break;

        case TOK_TOP:
          t = arg_SHIFT(pgm,op,t,TOK_T0P);
          break;

        case TOK_NXQ:
          t = arg_SHIFT(pgm,op,t,TOK_N0Q);
          break;

        default: throw(ERR_SYNTAX);
      }
      s = t;
    }
  
    skp(s,"';' *!n",&s);
    skp(s,"W",&s);
    throwif (*s != '\0' && *s != '\n', ERR_SYNTAX);
  }
  catchall {
    pgm->pgm_err = thrown();
    pgm->cur_col = (int32_t)(t-line) + thrownaux();
  }

  return pgm->pgm_err;
}

static int lblcmp(const void *a, const void *b)
{ uint32_t aa = *((uint64_t *)a) >> 32;
  uint32_t bb = *((uint64_t *)b) >> 32;
  return aa == bb? 0 : aa < bb ? -1 : 1 ;
}

static int32_t lbl2offset(kaj_pgm_t pgm, int32_t lbl)
{

  if (pgm->lst_type == LST_UNSORTED_LBL) {
    qsort(pgm->lst.lbl, pgm->lst_count,sizeof(uint64_t),lblcmp);
    pgm->lst_type = LST_SORTED_LBL;
    // Check duplicates
    for (int k=1; k<pgm->lst_count; k++) {
      throwif ((pgm->lst.lbl[k] & 0xFFFFFFFF00000000) == (pgm->lst.lbl[k-1] & 0xFFFFFFFF00000000) , ERR_DUPLICATE_LBL);
    }
  }

  int i,j,m;
  int32_t lbl_tmp;

  i = 0;
  j = pgm->lst_count-1;
 _dbgtrc("lbl->offset: %06X",lbl);
  while (i <= j) {
    m = (i+j)/2;
    lbl_tmp = pgm->lst.lbl[m] >> 32;
    if ( lbl_tmp == lbl) return pgm->lst.lbl[m] & 0x00FFFFFF;
    if (lbl_tmp < lbl) i = m+1;
    else j = m -1;
  }
  throw(ERR_UNDEFINED_LBL);
  return -1;
}

int kaj_assemble(kaj_pgm_t pgm)
{
  // The function is called "assemble" but at this point we only
  // have to fix forward labels and allocate the stack for JSR.

  int32_t  k = 0;
  uint32_t op;
  int32_t  lbl;
  int32_t  offset;

  if (pgm->lst_type == LST_REGISTERS) return ERR_EXECUTABLE_PGM;
  if (pgm->pgm_err != ERR_NONE) return pgm->pgm_err;
  
  if (pgm->pgm_flg & FLG_DATA) {
    pgm->pgm[pgm->dta_start] |= pgm->pgm_count << 8;
    pgm->pgm_flg &= ~FLG_DATA;
  }

  // add HLT at the end.
 _dbgtrc("count: %d",pgm->pgm_count);
  add_long(pgm,0);

  // Fix labels
  try {
    while (k<pgm->pgm_count) {
     _dbgtrc("ASM: %08X",k);
      op = pgm->pgm[k]; 

      switch (op & 0xFF) {
        case TOK_DTA :
          k = (op >> 8) -1 ;
         _dbgtrc("DTA: next %08X",k+1);
          break;

        case 0x80 | TOK_CP4:
        case 0x80 | TOK_ST4:
        case 0x80 | TOK_RT4:
        case 0x80 | TOK_CPI:
        case 0x80 | TOK_STI:
        case 0x80 | TOK_RTI:
        case 0x80 | TOK_CPN:
        case 0x80 | TOK_STN:
        case 0x80 | TOK_RTN:
          lbl = pgm->pgm[k+1];
          offset = lbl2offset(pgm,lbl);
          pgm->pgm[k] &= 0xFFFFFF7F;
          pgm->pgm[k+1] = offset;
          break;
  
        case 0x80 | TOK_JEQ :
        case 0x80 | TOK_JNE :
        case 0x80 | TOK_JGE :
        case 0x80 | TOK_JGT :
        case 0x80 | TOK_JLE :
        case 0x80 | TOK_JLT :
        case 0x80 | TOK_JMP :
        case 0x80 | TOK_JSR :
          lbl = (uint32_t)op >> 8;
          offset = lbl2offset(pgm,lbl);
          pgm->pgm[k] = (offset << 8) | (op & 0x7F);
          break;
      }
      k += kaj_opcode_len(op);
    }
    // We need no labels!
    // Let's allocate registers, instead! (with room for an additional 256 regs for SAV/RCL).
    if (pgm->max_regs == 0) pgm->max_regs = 1; // at least Register %00 must exist
    pgm->lst_size = pgm->max_regs+256;
    pgm->lst_count = pgm->max_regs+1; // Used as stak pointer.
    pgm->lst.regs = realloc(pgm->lst.lbl,sizeof(val_t)*(pgm->lst_size));
    throwif(pgm->lst.regs == NULL, ERR_NO_MEMORY);
    pgm->lst_type = LST_REGISTERS;

    // Now extend pgm to make room for enough stack space
    if (pgm->stk_size <=0) pgm->stk_size = 256;
    pgm->pgm_size = pgm->pgm_count+pgm->stk_size;
    pgm->pgm = realloc(pgm->pgm,pgm->pgm_size * sizeof(uint32_t));
    throwif(pgm->lst.regs == NULL, ERR_NO_MEMORY);
    pgm->max_pgm = pgm->pgm_count-1;
  }
  catchall {
    pgm->cur_ln = lbl;
    pgm->pgm_err = thrown();
    return pgm->pgm_err;
  }
  return ERR_NONE;
}

void kaj_perror(kaj_pgm_t pgm, char *line, FILE *errfile)
{
  int32_t err = pgm->pgm_err;
  if (err != ERR_NONE && errfile) {
    if (err == ERR_UNDEFINED_LBL || err == ERR_DUPLICATE_LBL) {
      fprintf(errfile,"ERROR: %s '@%X'\n",ERR_MSG[err], pgm->cur_ln);
    }
    else {
      fprintf(errfile,"ERROR: %s :%d:%d\n",ERR_MSG[pgm->pgm_err], pgm->cur_ln, pgm->cur_col);
      fprintf(errfile,"%s",line);
      if (*line == '\0' || line[strlen(line)-1] != '\n') fputc('\n',errfile);
      for (int k=0; k<pgm->cur_col;k++) fputc('~',errfile);
      fputc('^',errfile);
      fputc('\n',errfile);
    }
  }
}

int kaj_fromfile(kaj_pgm_t pgm, FILE *f, FILE *errfile)
{
  #define MAX_LINE_LEN 128
  static char line[MAX_LINE_LEN];
  int err = ERR_NONE;
  while (err == 0 && fgets(line, MAX_LINE_LEN, f)) {
    err = kaj_addline(pgm,line);
  }
  err = kaj_assemble(pgm);
  kaj_perror(pgm,line,errfile);
  return err;
}

