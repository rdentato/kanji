# kanji a bytecode VM that computes

WARNING: This doc is work in progress! It may or may not be 100% accurate. Please aks if doubt!

DISCORD group: https://discord.gg/vPgsxHcgXX
MAIL: rdentato@gmail.com

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
   vectors (can be also used as stacks or queues)
   buffers
   string (immutable)
   maps (not implemented yet)


## Constants

Constants can be defined with the `VAL` opcode:

```
   @001:  VAL 3.14       ; PI
          VAL "Hello!"
```

 Constant can be accessed throught the label:

 ```
           STO %1 @001[0] ; Store PI in %1
           STO %2 @001[1] ; Store (ptr to) "Hello!" in %2
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
   - Stored value: `STO %00 @43[0]`, `STO %00 @43[1]`
   
## Comparison
Comparison have the same syntax as the `STO` instruction:

   - `CMP %01 3`
   - `CMP %01 %5F`
   - `CMP %01 @001[1]`

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

Increment/Decremnent. 
```
 INC %01 n        ; %01 <- %01 + n
 DEC %01 n        ; %01 <- %01 - n
```

The `DEC` operation sets the `equal` flag when it reaches 0.
This is rather convenient for definite loops:

```
    STO %1 10
    STO %2 "Hello World\n"

@1: SYS io.print %0 %2
    DEC %1
    JNE @1
   
    RET  

```

Convert between Integers and Float:
```
 INT %01 %02      ; %01 <- (int)%02
 FLT %01 %02      ; %01 <- (double)%02
```

## Jump

The argument can be a label, an integer or a register (which
contains the address to jump to).

The jump operators have two forms:
```
  JJJ  n  ; an integer between 0 and 0xFFFFFF
  JJJ %2  ; a register
```

The Assembler will convert labels to integers and replace
them as arguments for the jump operators so that in, 
for example, `JMP @3`, the label `@3` will be replaced 
by its real offset.

```
 JEQ
 JNE
 JGT
 JGE
 JLT
 JLE
 JMP
```

This implies that the max size for a kanji progam is 16 millions
(actually 2^24) instructions (actually less considering the 
arguments, constants, etc.)

## Subroutines

Subroutines accept a single value as argument and return a
a single value (you can use a vector to pass, return multiple values)

The first argument is an integer or a register containing the offset
for the entry point of the subroutine (labels will be converted by
the assembler).

The first argument is the register where the return value (if any)
will be placed. The second is the input value for the subroutine.

```
  JSR n %1 %2
```

Within the subroutine you can retrieve the passed argument with 
the `ARG` operator.

```
     STO %1 100
     JSR @F1 %2 %1 ; <- 100 will be passed to the subroutine
                   ;    200 will be in %2 after return
     RET

@F1: SAV %0        ; save watever value is in %0
     ARG %0        ; <- %0 will contain 100
     ADD %0 %0 %0  ; multiply by 2
     RTV %0        ; Set the return value as the current value of %0
     RCL %0        ; Recall the saved value of %0
     RET
```

To avoid clash, you can explicitly save registers in reserved stack:

```
      STO %1 %2 %9    ; Save up to three registers with a single SAV
      ...
      RCL %9 %2 %1    ; Recall the registers (in reverse order!)
```

Since you can pass multiple arguments in a vector, it should not be 
needed to play with registers so much.

## Calling C functions

To call a C function in the system library, you use the `SYS` operator:

```
    STO %1 "Hello World!\n"
    SYS io.print %0 %1
    RET
```

It works exactly as the `JSR` operator but the first argument is the 
name of one of the C function that have been compiled into the
system library.

You can add C functions to be called by a kanji program. To do so,
you put an interface file in the `syslib` directory and rebuild the
interpreter.

The function must take a single `val_t` argument and must return
a `val_t` value.

For example, the `sin()` function the `math.c` file has the
interface:

```
val_t mth.sin(val_t arg)
{
  if (valisdbl(arg)) return val(sin(valtodbl(arg)));
  return valnil;
}
```

Note that the name of the function is written in a way to make it 
recognizable as a interface function (it would be an illegal name in C).
The function name is case insentive, may contain only letters, numbers 
and a mandatory dot (`.`). It can't be longer than 12 characters (dot included).

## Using kanji from C

To build a kanji program you can, of course, just write down each opcode 
in an array. However this may be more complicated than it sounds because
there are a lot of variation for each opcode.

For example, along with the `STO` opcode there are `ST4` to store 32 bit
integers, `ST8` to store `val_t` values, `STI` to pick a constant etc etc.

Also, you'll have to layout the constant strings appropriately, the registers
and so on. Rather cumbersome!

The easiest way is to use the provided assembler:

   - create a new program with `kaj_new()`
   - add the lines (as strings) one by one with `kaj_addline()`
   - finalize the assembly with `kaj_assemble()`
   - run the program with `kaj_run()`
   - destroy the program with `kaj_free()`

You can see this in action in the main of `src/kanji.c` and 
the `kaj_loadfromfile()` function in `src/kanji_asm.c`.
