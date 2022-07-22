#line 1 "parser.skp"
#include "skp.h"

  #include "keywords.h"

  skpfunc(keyword) {
    if (*skpcur < '@' ) {
      switch (*skpcur++) {
        case '+' : skpreturn(skpcur,KW_ADD) ;  
        case '*' : skpreturn(skpcur,KW_MULT) ;  
        case '-' : skpreturn(skpcur,KW_SUB) ;
        case '/' : skpreturn(skpcur,KW_DIV) ;
        case '<' : if (*skpcur == '=') skpreturn(skpcur+1, KW_LE); else skpreturn(skpcur, KW_LT) ;
        case '>' : if (*skpcur == '=') skpreturn(skpcur+1, KW_GE); else skpreturn(skpcur, KW_GT) ;
      }
      skpreturn(NULL,0);
    }

    if (*skpcur <= 'z') {
      int i,j,m,cmp;
      char candidate_kw[8];

     _skptrace("KW: '%.5s'",skpcur);
      for (i=0; i<8; i++) { 
       _skptrace("candidate: %d '%c'",i,*skpcur);
        if ('a' <= *skpcur && *skpcur <= 'z' ) {
          candidate_kw[i] = *skpcur; skpcur++; 
        }
        else if (*skpcur == '?') {
          candidate_kw[i] = *skpcur; skpcur++; 
          break;
        }
        else break;
      }
      if (i >= 8) {
        skpreturn(NULL,0);
      }
      candidate_kw[i] = '\0';

     _skptrace("candidate: '%s'",candidate_kw);

      i = 0; j = keywords_count-1;
     _skptrace("cand next: %d %d",i,j);
      while (i<=j) {
        m = (i+j)/2;
       _skptrace("cand test: '%s'",kwords+(m*8));
        cmp = strcmp(candidate_kw, kwords + (m * 8));
        if (cmp == 0) skpreturn(skpcur, m+1);
        if (cmp < 0) j = m-1;
        else i = m+1;
       _skptrace("cand next: %d %d",i,j);
      }

      skpreturn(NULL,0);
    }

    skpreturn(NULL,0);
  }

#line 59
skpdef(program) {   skpany {     skprule_(_);     skprule(expression);   }   skprule_(EOF_); }
#line 61
skpdef(expression) {   skprule_(POPEN);   skprule_(_);   skprule_(expr);   skprule_(PCLOSE); }
#line 63
skpdef(expr) {   skponce {     skprule(let);   }   skpor {     skprule(lambda);   }   skpor {     skpany {       skprule_(_);       skprule_(atom);     }   } }
#line 65
skpdef(let) {   skpstring_("let");   skprule_(SPC_);   skprule(letvar);   skprule_(SPC_);   skprule(letexpr); skpast(lift); }
#line 66
skpdef(letvar) {   skprule_(notkeyword); }
#line 67
skpdef(letexpr) {   skponce {     skprule(NUMBER);   }   skpor {     skprule(STRING);   }   skpor {     skprule(expression);     if (astfailed) { skpabort("Invalid expression"); }   } }
#line 69
skpdef(notkeyword) {   skpnot {     skpgroup {       skponce {         skpstring_("let");       }       skpor {         skprule_(LAMBDA);       }       skpor {         skpcheck(keyword);       }     }   }   if (astfailed) { skpabort("Can't reletine keyword"); }   skprule_(IDENTIFIER);   if (astfailed) { skpabort("Expcting identifier"); } }
#line 71
skpdef(lambda) {   skprule_(LAMBDA);   skprule_(_);   skprule(args);   skprule_(_);   skprule(body); }
#line 72
skpdef(body) {   skprule_(expression); }
#line 74
skpdef(args) {   skprule_(POPEN);   skpany {     skprule_(_);     skprule(arg);   }   skprule_(PCLOSE);   if (astfailed) { skpabort("missing lambda arugments"); } }
#line 75
skpdef(arg) {   skprule_(IDENTIFIER); }
#line 77
skpdef(atom) {   skpnot {     skpgroup {       skponce {         skpstring_("let");       }       skpor {         skprule_(LAMBDA);       }     }   }   skpgroup {     skponce {       skpcheck(keyword);     }     skpor {       skprule(NUMBER);     }     skpor {       skprule(STRING);     }     skpor {       skprule(IDENTIFIER);     }     skpor {       skprule(expression);     }   } }
#line 79
skpdef(NUMBER) {   skpmatch_("F"); }
#line 80
skpdef(STRING) {   skpmatch_("& '\"'");   skpmatch_("Q"); }
#line 81
skpdef(IDENTIFIER) {   skpmatch_("I");   skpmatch_("?[?!]"); }
#line 82
skpdef(NIL) {   skpmatch_("'(' S ')'"); }
#line 84
skpdef(POPEN) {   skpstring_("(");   skprule_(_); }
#line 85
skpdef(PCLOSE) {   skprule_(_);   skpstring_(")"); }
#line 87
skpdef(LAMBDA) {   skponce {     skpstring_("λ");   }   skpor {     skpstring_("lambda");   } }
#line 89
skpdef(_) {   skpany {     skponce {       skpmatch_("+s");     }     skpor {       skpmatch_("';' N");     }   } }
#line 90
skpdef(SPC_) {   skpmany {     skponce {       skpmatch_("+s");     }     skpor {       skpmatch_("';' N");     }   } }
#line 91
skpdef(EOF_) {   skprule_(_);   skpmatch_("!."); }
