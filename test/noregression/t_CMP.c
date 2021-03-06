//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#define DEBUG DEBUG_TEST
#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;
  val_t ret;

  dbgtst("CMP two of the same type") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STO %1 1 \n"
                  "    STO %0 3.14 \n"
                  "    CMP %0 3.14 \n"
                  "    JNE @1 \n"
                  "    STO %0 $CDEF9876 \n"
                  "    CMP %0 $CDEF9876 \n"
                  "    JNE @1 \n"
                  "    STO %0 \"pippo\" \n"
                  "    CMP %0 \"pippo\" \n"
                  "    JNE @1 \n"
                  "    RET    \n"
                  "@1: STO %1 2 \n" // We should get 1 in register 1
                  "    RET",
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

  dbgtst("CMP two registers") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STO %1 1 \n"
                  "    STO %0 3.14 \n"
                  "    STO %2 3.14 \n"
                  "    CMP %0 %2 \n"  // Compare two registers
                  "    JEQ @1 \n"
                  "    STO %1 2 \n" // We should get 1 in register 1
                  "@1: RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(3.14), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(1), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("CMP won't compile on syntax error") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "CMP %0\n"
                  "RET",
                NULL);
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err != 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == valnil, "Return error. Got: %lX",ret);

    kaj_free(pgm);

    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "CMP \n"
                  "RET",
                NULL);
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_init(pgm,0,0x00,val(0));
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err != 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == valnil, "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("CMP int and double (equality") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "    STO %1 1 \n"
                  "    STO %0 3 \n"    // Integer 3 and double 3.0 are the same number
                  "    STO %2 3.0 \n"
                  "    CMP %0 %2 \n" 
                  "    JNE @1 \n"
                  "    CMP %2 %0 \n" 
                  "    JNE @1 \n"
                  "    STO %2 3.1 \n"
                  "    CMP %0 %2 \n" 
                  "    JEQ @1 \n"
                  "    RET    \n"
                  "@1: STO %1 2 \n" // We should get 1 in register 1
                  "    RET",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(1), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  return 0;
}
