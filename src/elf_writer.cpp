
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
  printf("e_shoff=%u\n", elf_hdr.e_shoff);
  elf_hdr.e_shnum = elf_.sections().size();
  auto shstrpos = std::find_if(
      elf_.sections().begin(), elf_.sections().end(),
      [](auto& section) { return section->header().sh_type == SHT_STRTAB; });
  elf_hdr.e_shstrndx = shstrpos - elf_.sections().begin();
  elf_hdr = swap_elf_header(elf_hdr);

  os.write(reinterpret_cast<char*>(&elf_hdr), sizeof(elf_hdr));

  off_t section_offs = elf_.sections().size() * sizeof(Elf32_Shdr);
  // The first entry in the ELF section header table is NULL
  {
    Elf32_Shdr null_hdr;
    memset(&null_hdr, 0, sizeof(null_hdr));
    os.write(reinterpret_cast<char*>(&null_hdr), sizeof(null_hdr));
    section_offs += sizeof(null_hdr);
  }

  std::cout << "shstrtab size " << elf_.shStringTable().size() << std::endl;

  std::string section_names{};
  // Now write out the rest of the section headers and build up the section name
  // string table
  for (auto it = elf_.sections().begin(); it != elf_.sections().end(); it++) {
    const auto& section = *it;
    auto& hdr = section->header();
    section_names.append(section->name() + '\0');
    hdr.sh_name = it - elf_.sections().begin();
    hdr.sh_size = section->size();
    hdr.sh_offset = section_offs;
    section_offs += section->size();
    hdr = swap_section_header(hdr);
    os.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
  }

  // Iterate over the sections a second time and write them out
  for (auto it = elf_.sections().begin(); it != elf_.sections().end(); it++) {
    const auto& section = *it;
    section->write(os);
  }
}

size_t ELFWriter::populate_section_headers(size_t starting_offs) {
  size_t offs = starting_offs;
  auto& shstrtab = elf_.shStringTable();
  for (auto section_it = elf_.sections().begin();
       section_it != elf_.sections().end(); section_it++) {
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
