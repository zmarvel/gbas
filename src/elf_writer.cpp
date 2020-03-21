
#include "elf_writer.hpp"

using namespace GBAS;

void ELFWriter::write(std::ostream& os) {
  // There are some sections of headers that we need to fill out or verify as
  // we go. In the ELF header:
  // - e_shoff, the section header table's offset
  // - e_shnum, the number of entries in the section header table
  // - e_shstrndx, the section header table index of the section name string
  //   table's header
  // In section headers:
  // - sh_name, an index into the section name string table where the section's
  //   name is stored.
  // - sh_addr, the memory address where the section should reside
  // - sh_offset, the offset from the beginning of the file to the section
  // - sh_size, the size of the section in bytes
  //
  // Just as importantly, the section header table and section name string table
  // need to be constructed.

  // Let the section header table immediate follow the ELF header.
  auto& elf_hdr = elf_.header();
  elf_hdr.e_shoff = sizeof(elf_hdr);
  elf_hdr.e_shnum = elf_.sections().size();
}

size_t ELFWriter::populate_section_headers(size_t starting_offs) {
  size_t offs = starting_offs;
  auto& shstrtab = elf_.shStringTable();
  for (auto section_it = elf_.sections().begin();
       section_it != elf_.sections().end();
       section_it++) {
    auto& section = *section_it;
    auto hdr = section->header();

    // populate index of section's name
    const auto name_idx = shstrtab.strings().size();
    shstrtab.strings().push_back(section->name());
    hdr.sh_name = name_idx;

    // TODO hdr.sh_addr

    hdr.sh_offset = offs;

    hdr.sh_size = section->size();

    offs += hdr.sh_size;
  }
}
