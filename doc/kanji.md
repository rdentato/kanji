# kanji a bytecode VM that computes

## General architecture

kanji is *register based* virtual machine

## Instructions

Here kanji instructions are described from the view point of the syntax
accepted by the assembler. All the opcodes are listed in the latest chapter.

This is the general form of kanji instructions

```
  @123: STO %01 100 ; comment
  \___/ \_/ \_____/ \___ everything after the semicolon is a comment
   \      \       \_____ arguments (from 0 to 3)
    \      \____________ op code
     \__________________ label
```

## Labels

Labels are in the form of a `@` followed by an hexadecimal number 
between 0 and 0xFFFFFF

## Data types

   32 bit signed Integers
   double precision floating point
   48 bit pointers
   booleans (true/false)
   nil
   vectors
   buffers
   maps

## Constants

Constants can be defined with the `VAL` opcode:

```
   @001:  VAL 3.14  ; PI
```


## Memory/Register Instructions

To move values in/to register the basic instruction is:

```
   STO dest src
```
The *destination* is always a register

   - Register to register:  `STO %00 %01`
   - Constant to register: `STO %00 23` or `STO %01 NIL`
   - Label offset to register:  `STO %00 @43`
   - Stored value: `STO %00 @43[0]`, `STO %00 @43[0]`
   
## Comparison
Comparison have the same syntax as the `STO` instruction:

   - `CMP %01 3`
   - `CMP %01 %5F`

## Aritmetic operation

The general form is:

```
   OPR dest arg1 arg2
```

All three arguments must be registers.

```
 ADD %01 %02 %03  ; %01 <- %02 op %03
 SUB %01 %02 %03  ; %01 <- %02 op %03
 MUL %01 %02 %03  ; %01 <- %02 op %03
 DIV %01 %02 %03  ; %01 <- %02 op %03
 MOD %01 %02 %03  ; %01 <- %02 op %03
 AND %01 %02 %03  ; %01 <- %02 op %03
 ORR %01 %02 %03  ; %01 <- %02 op %03
 XOR %01 %02 %03  ; %01 <- %02 op %03
```

For these, the third argument can be an integer (between 0 and 63)

```
 ASR %01 %02 %03  ; %01 <- %02 >> %03 (signed)
 SHL %01 %02 %03  ; %01 <- %02 << %03
 SHR %01 %02 %03  ; %01 <- %02 >> %03 (unsigned)
```

```
 NOT %01 %02      ; %01 <- ~ %02    (binary not)
 NEG %01 %02      ; %01 <- %02 * -1 (2's complement)
```
```
 INC %01 n        ; %01 <- %01 + n
 DEC %01 n        ; %01 <- %01 - n
```
```
 INT %01 %02      ; %01 <- (int)%02
 FLT %01 %02      ; %01 <- (double)%02
 
```

## Jump

The argument can be a label, an integer or a register (which
contains the address to jump to).

```
 JEQ n
 JNE n
 JGT n
 JGE n
 JLT n
 JLE n
 JMP n
```

## Subroutines

Subroutines accept a single value as argument and return a
a single value (you can use a vector to pass, return multiple values)

```
  JSR 
```

## C functions

