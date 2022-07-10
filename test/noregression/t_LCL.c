//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#define DEBUG DEBUG_TEST
#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;
  val_t ret;

  dbgtst("LCL") {
    pgm = kaj_new(0,0,0);
    dbgmst(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "STO %1 1 \n"
                  "STO %2 2 \n"
                  "JSR @1 %1 %2 \n"
                  "RET \n"

              "@1: LCL 10 \n"
                  "LCL #2 %2\n"
                  "ADD %2 %1 %2 \n"
                  "LCL %1 #2\n"
                  "LCL \n"
                  "RET \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(2), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,2);
    dbgchk(ret == val(3), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("LCL (upval)") {
    pgm = kaj_new(0,0,0);
    dbgmst(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "LCL 5 \n"
                  "STO %1 1 \n"
                  "STO %2 2 \n"
                  "LCL #2 %1 \n"
                  "JSR @1 \n"
                  "LCL \n"
                  "RET \n"

              "@1: LCL 10 \n"
                  "LCL #2 %2\n"
                  "LCL %A #2 1 \n"
                  "LCL \n"
                  "RET \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(1), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,10);
    dbgchk(ret == val(1), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  return 0;
}
