
# TODO

## Assembler

- Relocation sections--I had prototyped an implementation but removed it because
  the implementatiof of the symbol table in `ELF` was wrong. There should be a
  relocation section for every other section (e.g. ".reltext").
- Lots more directives.
- More instruction types--currently partial support only.

## Testing

There are already lots of test programs and the expected tokenizer output for
each one. For at least a subset of these programs, develop a method for
providing them to the parser as input and checking the resulting AST. Then,
check that the object file produced by the assembler is correct as well.

For the parser, this could mean writing code to load an AST represented by
s-expressions into a tree of `Node`s. The code already exists in
`assembler_test.cpp` to compare ASTs.

For the assembler, I guess this means constructing some ELF files by hand. The
more flexible approach would be to define some notion of equality between two
ELF files so that two sections, each defining the same symbols but in a
different order, are considered "the same." The simpler approach would be a
bytewise comparison of the two files.
