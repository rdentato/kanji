/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

#define DEBUG DEBUG_TEST
#define VRGOPTS
#include "vrg.h"

#define DBG_MAIN
#include "dbg.h"

#define SKP_MAIN
#define VAL_MAIN
#include "parser.h"

void generate(ast_t ast);

/************************************/

#define MAXFNAME 120
static char fnamebuf[MAXFNAME+8];
static char bnamebuf[MAXFNAME];

static char *loadsource(char *fname)
{
  FILE *f;
  int32_t size=0;
  char *src=NULL;

  f = fopen(fname,"rb");
  if (f) {
    fseek(f,0,SEEK_END);
    size = ftell(f);
    fseek(f,0,SEEK_SET);
    src = malloc(size+1+!(size & 1));
    if (src) {
      *src = '\0';
      if (fread(src,size,1,f)) {
        src[size] = '\0';
      }
      else {
        free(src);
        src = NULL;
      };
    }
    fclose(f);
  }
  
  return src;
}

#define trace(...) (fprintf(stderr,__VA_ARGS__),fputc('\n',stderr))

int main(int argc, char *argv[])
{
  char *kajbuf = NULL;
  ast_t ast = NULL;
  FILE *src=NULL;
  FILE *hdr=NULL;
  char *s;
  int trace_enabled =0;
  char *fname="_expr_";

  vrgver("kajc (kanji compiler)\n"
         "v0.0.7-beta (C) 2022 Remo Dentato http://gh.dentato.com/kanji");

  vrgoptions(argc,argv) {
    vrgopt("-h\tPrint help and exit") {
      vrghelp();
    }
    
    vrgopt("-f filename\tRun source file") {
      fname = vrgoptarg;
      kajbuf = loadsource(fname);
    }

    vrgopt("-t\tEnable tracing in parser") {
      trace_enabled = 1;
    }

    vrgoptdefault {
      fprintf(stderr, "WARNING: ignored option -%c\n",argv[vrgargn][1]);
    }
  }

  if ((kajbuf == NULL) && (vrgargn < argc)) {
    fname = argv[vrgargn];
    kajbuf = loadsource(fname);
  }

  if (kajbuf == NULL)  vrgerror("Missing source file\n");

  ast = skpparse(kajbuf,prog,trace_enabled);

  if (asthaserr(ast)) {
    trace("In rule: '%s'",asterrrule(ast));
    char *ln = asterrline(ast);
    char *endln = ln;
    while (*endln && *endln != '\r' && *endln != '\n') endln++;
    int32_t col = asterrcolnum(ast);
    trace("Error: %s",asterrmsg(ast));
    trace("%.*s",(int)(endln-ln),ln);
    trace("%.*s^",col,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    return 1;
  }
  else {
    strcpy(bnamebuf,fname);
    s=bnamebuf;
    while (*s) s++;
    while (s>bnamebuf && s[-1] != '.') s--;
    if (s>bnamebuf) s[-1] = '\0';
    
    sprintf(fnamebuf,"%s"".t",bnamebuf);
    hdr = fopen(fnamebuf,"w");
    assert(hdr);

    //skptagrules();

    astprint(ast,hdr);

    // for (int k=0; k< ast->par_cnt; k++) {
    //   fprintf(stderr,"%d %d\n",k,ast->par[k]);
    // }

    fprintf(hdr,"%d nodes\n",astnumnodes(ast));
    fclose(hdr); hdr = NULL;
    fprintf(stderr,"AST file: '%s'\n",fnamebuf);

    generate(ast);

#if 0
    sprintf(fnamebuf,"%s"".c",bnamebuf);
    src = fopen(fnamebuf,"w");
    if (src) {
      generate(ast,src);
      fclose(src); src = NULL;
    }
    else {
      fprintf(stderr,"Unable to write on %s\n",fnamebuf);
    }
#endif    
  }

  if (src) fclose(src);
  if (hdr) fclose(hdr);
   
  if (kajbuf) free(kajbuf);
  if (ast)   astfree(ast);

  return 0;
}
