//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#define DEBUG DEBUG_TEST
#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;
  val_t ret;

  dbgtst("First RET will exit the program") {
    pgm = kaj_new(0,0,0);
    dbgmst(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "STO %1 1 \n"
                  "RET      \n"
                  "STO %1 2 \n"
                  "RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(1), "Return error. Got: %lX",ret);
  }
  return err;
}
