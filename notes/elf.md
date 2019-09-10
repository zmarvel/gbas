
# ELF files

## Relocation sections

- Relocation sections are tables of relocation entries. These store the offset
  of a symbol's data within its section *and* the index of the symbol in its
  symbol table.
- The relocation section's header stores the index of the corresponding symbol
  table's header in its `sh_link` member.
