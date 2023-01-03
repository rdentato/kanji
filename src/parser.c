#line 3 "parser.skp"
#include "parser.h"
#include "skp.h"

  #include "parser_aux.c"
  #include "parser_gen.c"


static int32_t asm_counter = 0;

#define startmain_act    mainnode = astcurnode; \
                         if (!sortfunc()) { \
                           astseterrmsg("duplicate function");\
                           skpast(fail);\
                         } \
                         if (mainnode > 1) asm_counter = 4;\
                         dbgtrc("LBL:%d @:%d",mainnode,asm_counter);

#define endprog_act      astsetlastinfo(mainnode); \
                         dbgtrc("LBL:%d @:%d",astcurnode-1,asm_counter); \
                         cleanfunc();

#define newfunc_act      if (!addfunc(astcurfrom,astcurnode-1)) skpast(fail);\
                         dbgtrc("LBL:%d",astcurnode-1);\
                         skpast(delete);

#define endfunc_act      while (astnodeis(astcur,astcurnode-1,return)) skpast(delete);
                         
#define checkloop_act    if (loopstk_cnt == 0) { \
                           astseterrmsg("break outside loop"); \
                           skpast(fail); \
                         }

#define checkfunc_act    if (infunc == 0) skpast(fail); 

#define fcall_act        int k; \
                         if ( (k = searchfunc(astcurfrom)) < 0) { \
                           astseterrmsg("undefined function"); \
                           skpast(fail); \
                         } \
                         else astretval( k );

#define loopstart_act    skpast(delete); \
                         loopstk_push(astcurnode-1); \
                         dbgtrc("LBL:%d",astcurnode-1);

#define loopend_act      astretval(loopstk_top()); \
                         loopstk_drop(); \
                         dbgtrc("LBL:%d @:%d",astcurnode+3,asm_counter);


#line 61
skpdef(prog) {   skpany {     skprule(fndef);   }    startmain_act  
  skprule_(block);   skprule(endprog);    endprog_act  
}
#line 65
skpdef(endprog) {   skprule_(_);   astseterrmsg("Unexpected code after end of program");  
  skpmatch_("!."); }
#line 68
skpdef(fndef) {   skprule_(FUNC);   skprule(fndef_name);   if (!astfailed) {  infunc = 1; newfunc_act;  }
  skprule_(block);     infunc = 0; 
  if (!astfailed) {  endfunc_act;  }
}
#line 71
skpdef(fndef_name) {   skprule_(fname); }
#line 72
skpdef(func_body) {   skprule_(block); }
#line 74
skpdef(block) {   skprule_(DO);   skpmany {     skprule_(stmt);     skprule_(_);   }   skprule_(END); }
#line 76
skpdef(stmt) {   skprule_(_);   skpgroup {     skponce {       skprule_(IF);     }     skpor {       skprule(block);     }     skpor {       skprule(loop);       if (!astfailed) {  astsettag(astcurnode+1) ; }
    }     skpor {       skpgroup {         skponce {           skprule(break);         }         skpor {           skprule(exit);         }         skpor {           skprule(return);         }       }       if (!astfailed) {  astsetempty;  }
    }     skpor {       skprule_(term);     }   } }
#line 83
skpdef(IF) {   skpstring_("?");   skprule(if);   if (!astfailed) {  astsettag(astcurnode+2) ; dbgtrc("LBL:%d",astcurnode+2); }
  skprule(else);   if (!astfailed) {  astsettag(astcurnode+1) ; dbgtrc("LBL:%d",astcurnode+1); }
  skprule(endif); }
#line 86
skpdef(if) {   skponce {     skprule_(block);   }   skpor {     skprule_(stmt);   } }
#line 87
skpdef(else) {   skpmaybe {     skprule_(_);     skpstring_(":");     skpgroup {       skponce {         skprule_(block);       }       skpor {         skprule_(stmt);       }     }   } }
#line 88
skpdef(endif) {    
}
#line 90
skpdef(loop) {   skprule(LOOP);   if (!astfailed) {  loopstart_act;  }
  skprule_(block);   skprule(loopend); }
#line 93
skpdef(loopend) {   if (!astfailed) {  loopend_act;  }
}
#line 95
skpdef(break) {    checkloop_act;  
  skprule_(BREAK);   if (!astfailed) {  astretval(loopstk_top());  }
}
#line 98
skpdef(return) {    checkfunc_act;  
  skprule_(RETURN); }
#line 101
skpdef(exit) {   skprule_(EXIT); }
#line 103
skpdef(term) {   skponce {      astseterrmsg("Unknow term");  
    skprule_(num);   }   skpor {     skprule(string);   }   skpor {     skpstring_("#");     skprule(label);     if (!astfailed) {  astdelete;  }
  }   skpor {     skpcheck(op);     if (!astfailed) {  asm_counter += 2;  }
  }   skpor {     skprule(syscall);     if (!astfailed) {  asm_counter += 10;  }
  }   skpor {     skprule(fcall);     if (!astfailed) {  asm_counter += 2;  }
  }   skpor {     skprule(getvar);   }   skpor {     skpstring_("@");     skpgroup {       skponce {         skprule(setdropvar);       }       skpor {         skprule(setvar);       }     }   } }
#line 113
skpdef(label) {   skpmatch_("D");   if (!astfailed) {  int32_t l=-strtol(astcurfrom,NULL,10); dbgtrc("LBL:%d @:%d", l, asm_counter);  }
}
#line 115
skpdef(getvar) {   skpmatch_("u");   if (!astfailed) {  astretval(*astcurfrom -'@');  }
  skprule_(index); }
#line 118
skpdef(setvar) {   skpmatch_("u");   if (!astfailed) {  astretval(*astcurfrom -'@');  }
  skprule_(index); }
#line 121
skpdef(setdropvar) {   skpstring_("!");   skprule_(setvar);   if (!astfailed) {  astretval(astcurfrom[1] -'@');  }
}
#line 123
skpdef(index) {   skpmaybe {     skpstring_("[");     skprule(ndxpr);     skprule_(_);     skpany {       skpstring_(",");       skprule(ndxpr);       skprule_(_);     }     skpstring_("]");   } }
#line 124
skpdef(ndxpr) {   skpany {     skprule_(_);     skprule_(term);   } }
#line 126
skpdef(fcall) {   skprule_(fname);   if (!astfailed) {  fcall_act;  }
}
#line 128
skpdef(syscall) {   skprule_(fname);   skprule_(fname);   if (!astfailed) {  astretval(addcall(astcurfrom,astcurptr,astcurnode));  }
}
#line 130
skpdef(fname) {   skpmatch_("'.' +[0-5a-z]"); }
#line 132
skpdef(num) {   skponce {     skprule(hex);   }   skpor {     skprule(float);   }   skpor {     skprule(int);   } }
#line 134
skpdef(hex) {   skpmatch_("'0' [Xx] X");   if (!astfailed) {  asm_counter += 6;  }
}
#line 135
skpdef(int) {   skpmatch_("D");   if (!astfailed) {  asm_counter += 6;  }
}
#line 136
skpdef(float) {   skpmatch_("D '.' *d");   if (!astfailed) {  asm_counter += 10;  }
}
#line 137
skpdef(string) {   skpmatch_("Q");   if (!astfailed) {  astretval(round2(astcurpos-astprevpos+1-2));  }
}
#line 139
skpdef(DO) {   skprule_(_);   skpmatch_("'do'     !& i");   skprule_(_); }
#line 140
skpdef(END) {   skprule_(_);   skpmatch_("'end'    !& i");   skprule_(_); }
#line 141
skpdef(FUNC) {   skprule_(_);   skpmatch_("'func'   !& i");   skprule_(_); }
#line 142
skpdef(LOOP) {   skprule_(_);   skpmatch_("'loop'   !& i");   skprule_(_); }
#line 143
skpdef(BREAK) {   skprule_(_);   skpmatch_("'break'  !& i");   skprule_(_); }
#line 144
skpdef(EXIT) {   skprule_(_);   skpmatch_("'exit'   !& i");   skprule_(_); }
#line 145
skpdef(RETURN) {   skprule_(_);   skpmatch_("'return' !& i");   skprule_(_); }
#line 147
skpdef(_) {   skpany {     skponce {       skpmatch_("+s");     }     skpor {       skpmatch_("'//' N");     }   } }
#line 148
skpdef(SPC_) {   skpmany {     skponce {       skpmatch_("+s");     }     skpor {       skpmatch_("'//' N");     }   } }

void skptagrules() {
  skpsetruletag(prog,skp_T_prog);
  skpsetruletag(endprog,skp_T_endprog);
  skpsetruletag(fndef,skp_T_fndef);
  skpsetruletag(fndef_name,skp_T_fndef_name);
  skpsetruletag(func_body,skp_T_func_body);
  skpsetruletag(block,skp_T_block);
  skpsetruletag(stmt,skp_T_stmt);
  skpsetruletag(IF,skp_T_IF);
  skpsetruletag(if,skp_T_if);
  skpsetruletag(else,skp_T_else);
  skpsetruletag(endif,skp_T_endif);
  skpsetruletag(loop,skp_T_loop);
  skpsetruletag(loopend,skp_T_loopend);
  skpsetruletag(break,skp_T_break);
  skpsetruletag(return,skp_T_return);
  skpsetruletag(exit,skp_T_exit);
  skpsetruletag(term,skp_T_term);
  skpsetruletag(op,skp_T_op);
  skpsetruletag(label,skp_T_label);
  skpsetruletag(getvar,skp_T_getvar);
  skpsetruletag(setvar,skp_T_setvar);
  skpsetruletag(setdropvar,skp_T_setdropvar);
  skpsetruletag(index,skp_T_index);
  skpsetruletag(ndxpr,skp_T_ndxpr);
  skpsetruletag(fcall,skp_T_fcall);
  skpsetruletag(syscall,skp_T_syscall);
  skpsetruletag(fname,skp_T_fname);
  skpsetruletag(num,skp_T_num);
  skpsetruletag(hex,skp_T_hex);
  skpsetruletag(int,skp_T_int);
  skpsetruletag(float,skp_T_float);
  skpsetruletag(string,skp_T_string);
  skpsetruletag(DO,skp_T_DO);
  skpsetruletag(END,skp_T_END);
  skpsetruletag(FUNC,skp_T_FUNC);
  skpsetruletag(LOOP,skp_T_LOOP);
  skpsetruletag(BREAK,skp_T_BREAK);
  skpsetruletag(EXIT,skp_T_EXIT);
  skpsetruletag(RETURN,skp_T_RETURN);
  skpsetruletag(_,skp_T__);
  skpsetruletag(SPC_,skp_T_SPC_);
}
