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

    kaj_free(pgm);
  }

  dbgtst("Check RETV (Double)") {
    pgm = kaj_new(0,0,0);
    dbgmst(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "STO %1 %0\n" // register 0 is in/out param
                  "ARG %2   \n" // You can retrieve it with ARG
                  "RTV 3.14 \n"
                  "RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(5));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(3.14), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(5), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,2);
    dbgchk(ret == val(5), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("Check RETV (string)") {
    pgm = kaj_new(0,0,0);
    dbgmst(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "ARG %1   \n" // You can retrieve it with ARG
                  "SYS io.print %0 %1 \n"
                  "RTV \"OK\" \n"
                  "RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val("pippo"));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(strcmp(valtostr(ret),"OK") == 0, "Return error. Got: %s",valtostr(ret));

    ret = kaj_getreg(pgm,1);
    dbgchk(strcmp(valtostr(ret),"pippo") == 0, "Return error. Got: %s",valtostr(ret));

    kaj_free(pgm);
  }

  return 0;
}
