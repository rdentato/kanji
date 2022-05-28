#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;

  pgm = kaj_new(0,0,0);
  
  err = kaj_fromfile(pgm,stdin,stderr);

  if (err == ERR_NONE) {
    kaj_dump(pgm,stdout);
    kaj_init(pgm,0,0x00,val(123));

    dbgtrc("maxreg: %d , pgmsize: %d (%d bytes)",pgm->max_regs, pgm->max_pgm+1, (pgm->max_pgm+1) *4);

    dbgtrc("pmg: %p",(void *)(pgm->pgm));
    dbgtrc("pgm stack: -1 %08X\n                 -2 %08X\n                 -3 %08X",pgm->pgm[pgm->pgm_count-1],pgm->pgm[pgm->pgm_count-2],pgm->pgm[pgm->pgm_count-3]);

    kaj_setreg(pgm,1,200);
    kaj_run(pgm,-1);

    val_t ret;
    ret = kaj_getreg(pgm,0);

    if (valisint(ret))
       dbgtrc("RET: (int) %d",valtoint(ret));
    else if (valisdbl(ret))
       dbgtrc("RET: (double) %g",valtodbl(ret));
    else if (valisstr(ret))
       dbgtrc("RET: (str) %s",(char *)valtoptr(ret));
    else if (valisptr(ret))
       dbgtrc("RET: (ptr) %p",valtoptr(ret));
    else dbgtrc("RET: %lX",ret);

    dbgtrc("FLG: %d", pgm->pgm_flg);

  }
  kaj_free(pgm);

  return err;
}
