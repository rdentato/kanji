//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#define DEBUG DEBUG_TEST
#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;
  val_t ret;

  dbgtst("JSR with label and two regs") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STO %1 5 \n"
                  "    STO %2 2 \n"
                  "    JSR @1 %1 %2 \n"
                  "    RTV %1 \n"
                  "    RET    \n"
                  "@1: ARG %3 \n"
                  "    ADD %3 %3 %3 \n"
                  "    RTV %3 \n"
                  "    RET \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(4), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(4), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("JSR with three regs") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STO %1 5 \n"
                  "    STO %2 2 \n"
                  "    STO %4 @1 \n"
                  "    JSR %4 %1 %2 \n"
                  "    RTV %1 \n"
                  "    RET    \n"
                  "@1: ARG %3 \n"
                  "    ADD %3 %3 %3 \n"
                  "    RTV %3 \n"
                  "    RET \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(4), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(4), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  return 0;
}
