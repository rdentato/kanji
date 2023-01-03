# Kanji programmer's manual

## Introduction

Kanji is a stack based language that is aimed to serve as a target language for other higher level scripting languages.


## Variables

There are 26 variable named `A` to `Z` and an implicit stack. 

Each variable and each cell of the stack can hold:

   - a 32 bit signed Integers
   - a double precision floating point
   - a 48 bit pointers
   - a boolean (true/false)
   - nil
   - a vector of values (integer indexed)
   - a stack of values (LIFO)
   - a queue of values (FIFO)
   - an immutable string
   - a buffer (mutable strings)
   - a map from value to value (not implemented yet)

## Structure  programs

## GRAMMAR RULES

```
prog = fndef* block

block = 'do' stmt+ 'end'

fndef = 'func' fname block

stmt = if / block / loop / 'break' / 'exit' / 'return' /
       num / string / op / fcall  / getvar / setvar

if = '?' then_stmt ':' else_stmt
then_stmt = stmt
else_stmt = stmt

loop = 'loop' _stmt

getvar =  variable ;
setvar = '@' '!'? variable ;

variable = varname index?  ;

varname = [A-Z]

index =  '[' ndxpr __ ( _',' __ ndxpr __ )* _']')? ;
ndxpr = (__ _term)+ ;

fcall = _fname ;

fname = (_"'.' +l")+ ;

num = hex / float / int ;

hex    = _"'0' [Xx] X" ;
float  = _"D '.' *d" ;
int    = _"D" ;
string = _"Q" ;

_     =    (_"+s" / _"'//' N")* ;
SPC_  =    (_"+s" / _"'//' N")+ ;
EOF_  = __ _"!." ;

```



