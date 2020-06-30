
#include <fstream>
#include <algorithm>

#include "elf_writer.hpp"

using namespace GBAS;

void ELFWriter::write(std::string path) {
  std::ofstream os{path};
  if (os.fail()) {
    std::stringstream ss;
    ss << "Failed to open " << path;
    throw ELFException{ss.str()};
  }

  write(os);
}

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
  auto shstrpos = std::find_if(
      elf_.sections().begin(), elf_.sections().end(),
      [](auto& section) { return section->header().sh_type == SHT_STRTAB; });
  elf_hdr.e_shstrndx = shstrpos - elf_.sections().begin() + 1;
  elf_hdr = swap_elf_header(elf_hdr);

  os.write(reinterpret_cast<char*>(&elf_hdr), sizeof(elf_hdr));

  // off_t section_offs = elf_.sections().size() * sizeof(Elf32_Shdr);
  off_t section_offs = sizeof(elf_hdr) + elf_.sections().size() * sizeof(Elf32_Shdr);
  // The first entry in the ELF section header table is NULL
  {
    Elf32_Shdr null_hdr;
    memset(&null_hdr, 0, sizeof(null_hdr));
    os.write(reinterpret_cast<char*>(&null_hdr), sizeof(null_hdr));
    section_offs += sizeof(null_hdr);
  }

  // Now write out the rest of the section headers and build up the section name
  // string table
  {
    uint32_t section_header_ctr = 0;
    uint32_t sh_name = 1;
    for (auto it = elf_.sections().begin(); it != elf_.sections().end(); it++) {
      const auto& section = *it;
      auto& hdr = section->header();
      hdr.sh_name = sh_name;
      // Add 1 to include null byte
      sh_name += section->name().size() + 1;
      hdr.sh_size = section->size();
      hdr.sh_offset = section_offs;
      section_offs += section->size();
      // printf("%s\t%08x\n", section->name().c_str(), hdr.sh_offset);
      hdr = swap_section_header(hdr);
      os.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
      section_header_ctr++;
    }

  }
  // Iterate over the sections a second time and write them out
  for (auto it = elf_.sections().begin(); it != elf_.sections().end(); it++) {
    const auto& section = *it;
    section->write(os);
  }
}
