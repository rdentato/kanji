
NOTE: the *destination* always comes before the source!

## Arithmetic
 - `...`  *dstreg* *reg* *reg* 

 - `ADD`  
 - `AND`  
 - `MOD`  
 - `MUL`  
 - `ORR`  
 - `SUB`  
 - `DIV`  
 - `XOR`  


## Negate
 - `...` *dstreg* *reg*
  
 - `NOT`  Binary or boolean NOT
 - `NEG`  Multiply by -1


## Conversion
 - `...` *dstreg* *srcreg*
  
 - `FLT` converts to a double floating point number
 - `INT` converts to an integer

## Increments

 - `...`  *reg* [*integer*]

 - `DEC` Decrement register *reg* by the specified amount (`1` by default)
 - `INC` Increment register *reg* by the specified amount (`1` by default)


## Shift
 - `...`  *reg* *reg* *reg or integer* 

 - `ASR`  Signed shift right
 - `SHR`  Unsigned shift right
 - `SHL`  Shift left

## Store values

 - `STO` *reg* *value* 

 *value* can be:
 - a register
 - a signed integer
 - a signed float (in the form xx.yy)
 - a string enclosed in double quotes
 - a label (`@21`)
 - a stored constant: a label followed by an offset in brackets (`@3A[4]`).

## Comparison

- `CMP` *reg* *value*

## Jumps

 - `...` *address*

 - `JEQ` Based on last `CMP`
 - `JGE`
 - `JGT`
 - `JLE`
 - `JLT`
 - `JNE`

 - `JMP` Unconditional jump

*address* can be:
 - a label
 - a register


 - `HLT` Stops the execution and returns immediately.

## Subroutine

 - `JSR` *address* [*result reg* [ *input reg* ]]
 - Calls the subroutine passing the current value of *input reg* and expecing the return value in the *result reg*

 - `ARG` *reg*  
 - Retrieves the input value passed by `JSR` and store it in *reg*

 - `RTV` *value*
 - Sets the value of the *result reg* specified by the invoking `JSR`
 
 - `RET` 
 - Returns to the instructions right after the invoking `JSR`


## System calls (C functions)

 - `SYS` *function* [*result reg* [ *input reg* ]]
 - Calls the function passing the current value of *input reg* and expecing the return value in the *result reg*

 *function* is a lower case identifier in the form `lib.func` (e.g. `io.print`).

## Save/Recall

You can save the value of registers on a stack and recall them when you need them

 - `SAV` *reg* [*reg* [*reg*]]
 - `RCL` *reg* [*reg* [*reg*]]

## No operation
 - `NOP`

## Constants
 You can store constant in the code and retrieve them through their address (label)

 - `VAL` *value*  (but not the reference to another constant!)

## Strings
 Constant strings are stored in the code as a sequence of characters seprated by `\0`.
 You can retrieve any of them if you know the offset

 - `STR` *reg or int*

## Data structures
  WARNING: Maps are not implemented yet

 Dynamically sized data structures but you can specify an initial size.

 - `VEC` [*integer*] 
 - `BUF` [*integer*]
 - `MAP` [*integer*]
 - `STK` [*integer*]
 - `QUE` [*integer*]

Access single element by index (not for STK or QUE!)
 - `SET` *reg* *index* *value*
 - `GET` *reg* *reg* *index*

Stacks:

 - `TOP` *reg* *reg* [ *index* ]
 - `PSH` *reg* *value*
 - `DRP` *reg* [ *index* ]

`TOP` can peek in the stack below the topmost element through an index.
`DRP` can drop more elements at once

 - `NXQ` *reg* *reg* [ *index* ]        Next value in the queu
 - `ENQ` *reg* *value*
 - `DEQ` *reg* [ *index* ]

Manage the structure:

 - `CLR` *reg*         wipes the structure contents
 - `KLL` *reg*         deallocate the structure
 - `LEN` *reg* *reg*   Number of elements in the structure
 - `SZE` *reg* *reg*   Max capacity of the structure

## Expressions

 - `XPR` *reg* *reg or string* evaluate an RPN expression.
