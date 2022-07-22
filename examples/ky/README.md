# `ky` a `kanji` language.

The language `ky` (which means *a word starting with the letter `k` that I previously referred to*)
is a simple scripting language whose only purpose is to demonstate using `kanji` as a compiler target.



program = global_var* function* code ;



global_var = 'var' var ;
var = simple_var / array_var ;
simple_var = var_name;
array_var = var_name '[' index ']' ;
function = 'function' fname '(' arg (',' arg)* ')' block;
block = 'begin' instr* 'end';
instr = block / assignment / if / while;

assignment = var ':=' expression
arg = expression
if = 'if' expression 'then' instr ('else' instr)?
while = 'while' expression instr

expression = term (opadd term)*



var pippo[23]
var pluto

function topolino(tip, tap)
begin
  global pippo

  pippo[3] = tip
  pippo[6] = tap

  topolino = "done"
end

writeln("Hello World!")



