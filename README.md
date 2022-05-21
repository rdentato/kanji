# kanji

The purpose of **kanji** (which translates in English as "*x<sub>1</sub> computes x<sub>2</sub> from data x<sub>3</sub> by process x<sub>4</sub>*")
is to provide an easy and flexible target for programming languages.
  
It's a simple CPU-like machine with up to 254 registers, each register can hold a *value* which can be:

   - a 32 bit signed integer
   - a double precision floating point
   - a 48 bit pointer (good for 64-bit platforms)
   - a boolean (_true_/_false_)
   - the value _nil_
   - a static string
   - a vector of *values*
   - a dynamic buffer (for text)
   - a map from *values* to *values*

It can access external C functions (with the proper interface) and its code can "called" from C functions.

