//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#define DEBUG DEBUG_TEST
#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;
  val_t ret;

  dbgtst("STK ") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STK %1 \n"
                  "    PSH %1 45 \n"
                  "    LEN %B %1 \n"
                  "    TOP %A %1 1 \n"
                  "    DRP %1 \n"
                  "    LEN %C %1 \n"
                  "    KLL %1\n"
                  "    RTV %A\n"
                  "    RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == valnil, "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,10);
    dbgchk(ret == val(45), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,11);
    dbgchk(ret == val(1), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,12);
    dbgchk(ret == val(0), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("STK 1") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STK %1 \n"
                  "    PSH %1 45 \n"
                  "    PSH %1 47 \n"
                  "    LEN %E %1 \n"
                  "    TOP %A %1 1 \n"
                  "    TOP %B %1 2 \n"
                  "    DRP %1 2\n"
                  "    LEN %C %1 \n"
                  "    PSH %1 49 \n"
                  "    TOP %D %1 \n"
                  "    KLL %1\n"
                  "    RTV %A\n"
                  "    RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == valnil, "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,10);
    dbgchk(ret == val(47), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,11);
    dbgchk(ret == val(45), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,14);
    dbgchk(ret == val(2), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,13);
    dbgchk(ret == val(49), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,12);
    dbgchk(ret == val(0), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  return 0;
}
