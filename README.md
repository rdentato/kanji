# kanji

**kanji** (which translates in English as "*x<sub>1</sub> computes x<sub>2</sub> from data x<sub>3</sub> by process x<sub>4</sub>*")
is an assembler-like scripting language that is intended to be 
used as a target for transpiling higher level interpreted languages.

It may be useful in the early stages of language development if you want
to have a running (albeit not final) implementation of your language while
still working on the syntax/semantic aspects.
Should the perfomance prove themselves adequate to your use case, 
nothing prevents you to use it as the final run-time.

  
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

It can access external C functions (with the proper interface) and its code can be "called" from C functions.

You can check what is planned/in progress/done [on the project kanban page](https://github.com/users/rdentato/projects/1).

I really welcome any question, suggestion, and help offer you may have.
Join the <a href="https://discord.gg/vPgsxHcgXX"><img src="https://github.com/rdentato/skp/blob/master/docs/Discord_button.jpg?raw=true" title="Discord Channel"></a> to discuss about them.

