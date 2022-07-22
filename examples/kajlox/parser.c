#line 1 "parser.skp"
#include "skp.h"

  #include "op_toks.h"

#line 14
skpdef(program) {   skpany {     skprule_(_);     skprule_(declaration);   }   skprule_(EOF_); }
#line 21
skpdef(declaration) {   skponce {     skprule(classDecl);   }   skpor {     skprule(funDecl);   }   skpor {     skprule(varDecl);   }   skpor {     skprule_(statement);   } }
#line 26
skpdef(classDecl) {   skpstring_("class");   skprule_(SPC_);   skprule(classdef);   skpmaybe {     skprule_(_);     skpstring_("<");     skprule_(_);     skprule(classparent);   }   skprule_(_);   skpstring_("{");   skprule(classbody);   skprule_(_);   skpstring_("}"); }
#line 29
skpdef(classbody) {   skpany {     skprule_(_);     skprule(function);   } }
#line 30
skpdef(funDecl) {   skpstring_("fun");   skprule_(SPC_);   skprule(function); }
#line 31
skpdef(varDecl) {   skpstring_("var");   skprule_(SPC_);   skprule(vardef);   skpmaybe {     skprule_(_);     skpstring_("=");     skprule_(expression);   }   skprule_(_);   skpstring_(";"); }
#line 33
skpdef(classdef) {   skprule_(IDENTIFIER); }
#line 34
skpdef(vardef) {   skprule_(IDENTIFIER); }
#line 35
skpdef(classparent) {   skprule_(IDENTIFIER); }
#line 41
skpdef(statement) {   skprule_(_);   skpgroup {     skponce {       skprule(exprStmt);     }     skpor {       skprule(forStmt);     }     skpor {       skprule(ifStmt);     }     skpor {       skprule(printStmt);     }     skpor {       skprule(returnStmt);     }     skpor {       skprule(whileStmt);     }     skpor {       skprule(block);     }   } }
#line 50
skpdef(exprStmt) {   skprule_(expression);   skprule_(_);   skpstring_(";"); }
#line 51
skpdef(forStmt) {   skpstring_("for");   skprule_(_);   skpstring_("(");   skprule_(_);   skpgroup {     skponce {       skprule(varDecl);     }     skpor {       skprule(exprStmt);     }     skpor {       skpstring_(";");     }   }   skpmaybe {     skprule_(expression);   }   skprule_(_);   skpstring_(";");   skpmaybe {     skprule_(expression);   }   skprule_(_);   skpstring_(")");   skprule_(statement); }
#line 54
skpdef(ifStmt) {   skpstring_("if");   skprule_(_);   skpstring_("(");   skprule_(expression);   skprule_(_);   skpstring_(")");   skprule_(statement);   skpmaybe {     skprule_(_);     skpmatch_("'else' !& i");     skprule_(statement);   } }
#line 56
skpdef(printStmt) {   skpmatch_("'print' !& i");   skprule_(expression);   skprule_(_);   skpstring_(";"); }
#line 57
skpdef(returnStmt) {   skpmatch_("'return' !& i");   skpmaybe {     skprule_(expression);   }   skprule_(_);   skpstring_(";"); }
#line 58
skpdef(whileStmt) {   skpstring_("while");   skprule_(_);   skpstring_("(");   skprule_(expression);   skprule_(_);   skpstring_(")");   skprule_(statement); }
#line 59
skpdef(block) {   skpstring_("{");   skprule(blkbody);   skprule_(_);   skpstring_("}"); }
#line 61
skpdef(blkbody) {   skpany {     skprule_(_);     skprule_(declaration);   } }
#line 73
skpdef(expression) {   skprule_(_);   skprule_(assignment); }
#line 75
skpdef(assignment) {   skponce {     skprule(assign_lhs);     skprule_(_);     skpstring_("=");     skprule(assign_rhs);   }   skpor {     skprule(logic_or); skpast(liftall);   } }
#line 78
skpdef(assign_lhs) {   skprule(dotvalue); }
#line 79
skpdef(assign_rhs) {   skprule_(expression); }
#line 81
skpdef(logic_or) {   skprule(logic_and); skpast(liftall);   skpany {     skprule(OP_OR);     skprule(logic_and); skpast(lift);   } }
#line 82
skpdef(logic_and) {   skprule(equality); skpast(liftall);   skpany {     skprule(OP_AND);     skprule(equality); skpast(lift);   } }
#line 83
skpdef(equality) {   skprule(comparison); skpast(liftall);   skpany {     skprule(OP_EQ);     skprule(comparison); skpast(lift);   } }
#line 84
skpdef(comparison) {   skprule(term); skpast(liftall);   skpany {     skprule(OP_CMP);     skprule(term); skpast(lift);   } }
#line 85
skpdef(term) {   skprule(factor); skpast(liftall);   skpany {     skprule(OP_ADD);     skprule(factor); skpast(lift);   } }
#line 86
skpdef(factor) {   skprule(unary); skpast(liftall);   skpany {     skprule(OP_MULT);     skprule(unary); skpast(lift);   } }
#line 88
skpdef(unary) {   skprule_(_);   skpgroup {     skponce {       skprule(OP_UNARY);       skprule(unary);     }     skpor {       skprule(dotvalue);     }   } }
#line 90
skpdef(dotvalue) {   skpgroup {     skponce {       skprule(fcall);     }     skpor {       skprule(primary);     }   }   skpany {     skpstring_(".");     skpgroup {       skponce {         skprule(fcall);       }       skpor {         skprule(primary);       }     }   } }
#line 91
skpdef(fcall) {   skprule_(primary);   skprule_(_);   skpstring_("(");   skprule(arguments);   skprule_(_);   skpstring_(")"); }
#line 93
skpdef(primary) {   skprule_(_);   skpgroup {     skponce {       skprule(TRUE);     }     skpor {       skprule(FALSE);     }     skpor {       skprule(NIL);     }     skpor {       skprule(THIS);     }     skpor {       skprule(NUMBER);     }     skpor {       skprule(STRING);     }     skpor {       skpstring_("(");       skprule(expression);       skprule_(_);       skpstring_(")");     }     skpor {       skprule(SUPER);       skpstring_(".");       skprule(IDENTIFIER);     }     skpor {       skprule(IDENTIFIER);     }   } }
#line 98
skpdef(OP_OR) {   skprule_(_);   skpmatch_("'or' !& i"); }
#line 99
skpdef(OP_AND) {   skprule_(_);   skpmatch_("'and' !& i"); }
#line 100
skpdef(OP_EQ) {   skprule_(_);   skpgroup {     skponce {       skpstring_("!=");       if (!astfailed) {         astretval(TOK_NEQ);       }     }     skpor {       skpstring_("==");       if (!astfailed) {         astretval(TOK_EQ);       }     }   } }
#line 101
skpdef(OP_CMP) {   skprule_(_);   skpgroup {     skponce {       skpstring_(">=");       if (!astfailed) {         astretval(TOK_GE);       }     }     skpor {       skpstring_(">");       if (!astfailed) {         astretval(TOK_GT);       }     }     skpor {       skpstring_("<=");       if (!astfailed) {         astretval(TOK_LE);       }     }     skpor {       skpstring_("<");       if (!astfailed) {         astretval(TOK_LT);       }     }   } }
#line 102
skpdef(OP_ADD) {   skprule_(_);   skpgroup {     skponce {       skpstring_("-");       if (!astfailed) {         astretval(TOK_SUB);       }     }     skpor {       skpstring_("+");       if (!astfailed) {         astretval(TOK_ADD);       }     }   } }
#line 103
skpdef(OP_MULT) {   skprule_(_);   skpgroup {     skponce {       skpstring_("/");       if (!astfailed) {         astretval(TOK_DIV);       }     }     skpor {       skpstring_("*");       if (!astfailed) {         astretval(TOK_MULT);       }     }   } }
#line 104
skpdef(OP_UNARY) {   skprule_(_);   skpgroup {     skponce {       skpstring_("!");       if (!astfailed) {         astretval(TOK_NOT);       }     }     skpor {       skpstring_("-");       if (!astfailed) {         astretval(TOK_NEG);       }     }   } }
#line 106
skpdef(TRUE) {   skpstring_("true"); }
#line 107
skpdef(FALSE) {   skpstring_("false"); }
#line 108
skpdef(NIL) {   skpstring_("nil"); }
#line 109
skpdef(THIS) {   skpstring_("this"); }
#line 110
skpdef(SUPER) {   skpstring_("super"); }
#line 117
skpdef(function) {   skprule(funcdef);   skprule_(_);   skpstring_("(");   skprule_(_);   skprule(parameters);   skprule_(_);   skpstring_(")");   skprule_(_);   skprule(block); }
#line 118
skpdef(parameters) {   skpmaybe {     skprule(parm);   }   skpany {     skprule_(_);     skpstring_(",");     skprule_(_);     skprule(parm);   } }
#line 119
skpdef(arguments) {   skpmaybe {     skprule_(expression);   }   skpany {     skprule_(_);     skpstring_(",");     skprule_(expression);   } }
#line 121
skpdef(funcdef) {   skprule_(IDENTIFIER); }
#line 122
skpdef(parm) {   skprule_(IDENTIFIER); }
#line 130
skpdef(NUMBER) {   skponce {     skpmatch_("+d ?'.' *d");   }   skpor {     skpmatch_("'.' +d");   } }
#line 131
skpdef(STRING) {   skpmatch_("Q"); }
#line 132
skpdef(IDENTIFIER) {   skpmatch_("I"); }
#line 134
skpdef(_) {   skpany {     skponce {       skpmatch_("+s");     }     skpor {       skpmatch_("'//'N");     }   } }
#line 135
skpdef(SPC_) {   skpmany {     skponce {       skpmatch_("+s");     }     skpor {       skpmatch_("'//'N");     }   } }
#line 137
skpdef(EOF_) {   skprule_(_);   skpmatch_("!."); }
