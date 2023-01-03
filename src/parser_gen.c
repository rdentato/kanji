#include "parser.h"

static int32_t asm_curlbl=0;

#define printasm(...)  (fprintf(asm_out,"%4d:  ",astcurnode),fprintf(asm_out,__VA_ARGS__), fputc('\n',asm_out))
#define printasm_(...) (fprintf(asm_out,"    :  ",astcurnode),fprintf(asm_out,__VA_ARGS__), fputc('\n',asm_out))

void generate(ast_t ast)
{
  FILE *asm_out;
  
  asm_out = fopen("kajasm.s","wb");
  assert(asm_out!=NULL);
  
  astvisit(ast) {
    astonentry {
      astcase(prog) {
        if (astcurtag > 1) printasm("JMP %d",astcurtag);
      }
      astcase(int) {
        int32_t n = strtol(astcurfrom,NULL,10);
        printasm("CONST INT %d",n);
      }
      astcase(op) {
        printasm("OP %d",astcurtag);
      }
      astcase(float) {
        float n = strtod(astcurfrom,NULL);
        printasm("CONST FLT %f",n);
      }
      astcase(if) {
        printasm("JNZ %d",astcurtag);
      }
      astcase(else) {
        printasm("JMP %d",astcurtag);
      }
    }
    astonexit {
      astcase(fndef) {
        printasm("RET");
      }
    }
  }
  // astprintsexpr(ast,asm_out);

  fclose(asm_out);

}