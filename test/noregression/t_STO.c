//  (C) by Remo Dentato (rdentato@gmail.com)
//  License: https://opensource.org/licenses/MIT

#define DEBUG DEBUG_TEST
#include "kanji.h"

int main(int argc, char *argv[])
{
  int err = 0;
  kaj_pgm_t pgm;
  val_t ret;

  dbgtst("STO compiles correctly") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "STO %0 3.14\n"        // Works with double?
                  "STO %1 2\n"           // Works with integers?
                  "STO %2 \"pippo\"\n"   // Works with strings?
                  "STO %3 %1\n"          // Move from reg to reg?
                  "RET",
                stderr);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(3.14), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(2), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,3);
    dbgchk(ret == val(2), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,2);
    dbgmst(valisstr(ret),"Return err. got %lX",ret);
    dbgchk((strcmp(valtostr(ret),"pippo") == 0), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("STO won't compile and won't run on syntax error") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "STO %0\n"
                  "RET",
                NULL);
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err != 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == valnil, "Return error. Got: %lX",ret);

    kaj_free(pgm);

        pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "STO \n"
                  "RET",
                NULL);
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err != 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err != 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == valnil, "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("STO retrieves constant data") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "     STO %0 @10[0] \n"
                  "     STO %1 @10[1] \n"
                  "     STO %2 @10[2] \n"
                  "@10: VAL 3.14      \n"
                  "     VAL 2         \n"
                  "     VAL \"pippo\" \n"
                  "     RET           \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(3.14), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(2), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,2);
    dbgmst(valisstr(ret),"Return err. got %lX",ret);
    dbgchk((strcmp(valtostr(ret),"pippo") == 0), "Return error. Got: %lX",ret);

    kaj_free(pgm);

    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "     STO %3 0       \n"
                  "     STO %0 @10[%3] \n"
                  "     INC %3         \n"
                  "     STO %1 @10[%3] \n"
                  "     INC %3         \n"
                  "     STO %2 @10[%3] \n"
                  "@10: VAL 3.14       \n"
                  "     VAL 2          \n"
                  "     VAL \"pippo\"  \n"
                  "     RET            \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret == val(3.14), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(2), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,2);
    dbgmst(valisstr(ret),"Return err. got %lX",ret);
    dbgchk((strcmp(valtostr(ret),"pippo") == 0), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  dbgtst("STO retrieves labels") {
    pgm = kaj_new(0,0,0);

    dbgchk(pgm, "Can't create kanji program");

    err = kaj_fromstring( pgm,
                  "     STO %0 @10  \n"
                  "     STO %1 1    \n"
                  "     JMP %0      \n"
                  "     RET         \n"
                  "@10: STO %1 3.14 \n"
                  "     RET         \n",
                NULL);
    dbgchk(err == 0, "Got error: %d",err);
   
    err = kaj_init(pgm,0,REG_NONE,val(0));
    dbgchk(err == 0, "Got error: %d",err);

    err = kaj_run(pgm,0);
    dbgchk(err == 0, "Got error: %d",err);

    ret = kaj_getreg(pgm,0);
    dbgchk(ret != val(0), "Return error. Got: %lX",ret);

    ret = kaj_getreg(pgm,1);
    dbgchk(ret == val(3.14), "Return error. Got: %lX",ret);

    kaj_free(pgm);
  }

  return 0;
}
