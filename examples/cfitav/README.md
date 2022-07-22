A simple lisp variant 

Everything is a function.

Numbers

Numbers are double floating points 
Arithmetic operations: `+`, `-`, `*`, `/`

`number?`
`zero?`

Booleans

`true`, `false`

`boolean?`

Comparison:  `=`, `<`, `<=`, `>`, `>=`:
`if` `and` `or` `not`

List 

`nil` empty list
`(nil?)` returns `true` if it's the empty list

`(list ...)` returns a list containing the supplied arguments:

`(headl l)` returns the first elemennt
`(tail l)` returns the list without its head.


Output operations


(lambda params expr...) creates an anonymous function with parameters params and body expr.... params can be nil, a symbol, or a list of symbols. If params is a symbol or a dotted list of symbols, the function will accept a variable number of arguments:

(print x) prints x to the standard output:
