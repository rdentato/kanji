#include "skp.h"
#include "dbg.h"

/*

program = global_vars* function* code ;
global_vars = 'var' vardef ;
vardef = simple_var ('=' simple_init)/ array_var / record_var;
simple_var = var_name;
array_var = var_name '[' index ']' ;
record_var = var_name '{' fields'}'
function = 'function' fname '(' arg (',' arg)* ')' block;
block = 'begin' instr* 'end';
instr = block / assignment / if / while;
assignment = var ':=' expression
arg = expression
if = 'if' expression 'then' instr ('else' instr)?
while = 'while' expression instr
expression = bool (bolop bool)*
bool = comp (relop comp)*
comp = term (opadd term)*
term = fact (opmult fact)*
fact = neg fact / number / fcall / varref

fcall = fname '(' (arg (',' arg))) ? ')'

relop = '<=' / '<' / '==' / '>=' / '>'
opadd = '+' / '-'/
opmult = '*' / '/'
boolop = 'and' / 'or'

*/

