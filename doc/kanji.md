# kanji a bytecode VM that computes

## General architecture

kanji is *register based* virtual machine

## Instructions

```
  @123: STO %01 100 ; comment
  \___/ \_/ \_____/ \___ everything after the semicolon is a comment
   \      \       \_____ arguments (from 0 to 3)
    \      \____________ op code
     \__________________ label
```
     

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

## Memory/Register Instructions

   STO dest src

   STO %00 %01
   STO %00 23
   STO %00 @43
   STO %00 @43[5]
   STO %00 @43[%01]

## Comparison

## Aritmetic operation

## Flow control

## C functions

