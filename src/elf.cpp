#include <algorithm>
#include <sstream>
#include <string_view>

#include "elf.hpp"
#include "swap_helpers.hpp"

using namespace GBAS;

#undef DEBUG_ELF
#ifndef DEBUG_GBAS
#define DEBUG_ELF
#else
#define DEBUG_ELF
#endif

#ifdef DEBUG_ELF
#define ELF_EXCEPTION(msg)         \
  do {                             \
    std::cerr << msg << std::endl; \
    throw ELFException{msg};       \
  } while (0)
#else
#define ELF_EXCEPTION(msg)   \
  do {                       \
    throw ELFException{msg}; \
  } while (0)
#endif

static const int ABIVERSION = 0;
static const unsigned char ELF_IDENT[] = {
    ELFMAG0,
    ELFMAG1,
    ELFMAG2,
    ELFMAG3,
    ELFCLASS32,
    ELFDATA2MSB,
    EV_CURRENT,
    ELFOSABI_STANDALONE,
    // EI_PAD
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

template <>
Elf32_Ehdr GBAS::swap_elf_header(Elf32_Ehdr& hdr) {
  Elf32_Ehdr swapped;
  memcpy(swapped.e_ident, hdr.e_ident, sizeof(swapped.e_ident));
  swapped.e_type = swap(hdr.e_type);
  swapped.e_machine = swap(hdr.e_machine);
  swapped.e_version = swap(hdr.e_version);
  swapped.e_entry = swap(hdr.e_entry);
  swapped.e_phoff = swap(hdr.e_phoff);
  swapped.e_shoff = swap(hdr.e_shoff);
  swapped.e_flags = swap(hdr.e_flags);
  swapped.e_ehsize = swap(hdr.e_ehsize);
  swapped.e_phentsize = swap(hdr.e_phentsize);
  swapped.e_phnum = swap(hdr.e_phnum);
  swapped.e_shentsize = swap(hdr.e_shentsize);
  swapped.e_shnum = swap(hdr.e_shnum);
  swapped.e_shstrndx = swap(hdr.e_shstrndx);
  return swapped;
}

template <>
Elf32_Shdr GBAS::swap_section_header(Elf32_Shdr& hdr) {
  Elf32_Shdr swapped;
  swapped.sh_name = swap(hdr.sh_name);
  swapped.sh_type = swap(hdr.sh_type);
  swapped.sh_flags = swap(hdr.sh_flags);
  swapped.sh_addr = swap(hdr.sh_addr);
  swapped.sh_offset = swap(hdr.sh_offset);
  swapped.sh_size = swap(hdr.sh_size);
  swapped.sh_link = swap(hdr.sh_link);
  swapped.sh_info = swap(hdr.sh_info);
  swapped.sh_addralign = swap(hdr.sh_addralign);
  swapped.sh_entsize = swap(hdr.sh_entsize);
  return swapped;
}

ELF::ELF() : curr_section_{0}, curr_rel_idx_{0} {
  // File header
  memcpy(&header_.e_ident, ELF_IDENT, EI_NIDENT);
  header_.e_type = ET_REL;
  header_.e_machine = EM_NONE;
  header_.e_version = EV_CURRENT;
  header_.e_entry = 0;
  header_.e_phoff = 0;
  header_.e_shoff = 0;  // set this later
  header_.e_flags = 0;
  header_.e_ehsize = sizeof(Elf32_Ehdr);
  header_.e_phentsize = sizeof(Elf32_Phdr);
  header_.e_phnum = 0;
  header_.e_shentsize = sizeof(Elf32_Shdr);
  header_.e_shnum = 0;  // set this later

  // Section header for section names string table
  shstrtab_idx_ = sections_.size();
  add_section(
      std::make_unique<StrTabSection>(
          "shstrtab",
          Elf32_Shdr{
              // This section's name will be first in the section names table
              .sh_name = 0,
              .sh_type = SHT_STRTAB,
              .sh_flags = 0,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // Section header for string table
  strtab_idx_ = sections_.size();
  add_section(
      std::make_unique<StrTabSection>(
          "strtab",
          Elf32_Shdr{
              // This section's name will be first in the section names table
              .sh_name = 0,
              .sh_type = SHT_STRTAB,
              .sh_flags = 0,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // data section
  add_section(
      std::make_unique<ProgramSection>(
          "data",
          Elf32_Shdr{
              .sh_name = 0,
              .sh_type = SHT_PROGBITS,
              .sh_flags = SHF_ALLOC | SHF_WRITE,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // rodata section
  add_section(
      std::make_unique<ProgramSection>(
          "rodata",
          Elf32_Shdr{
              .sh_name = 0,
              .sh_type = SHT_PROGBITS,
              .sh_flags = SHF_ALLOC,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // bss section
  add_section(
      std::make_unique<ProgramSection>(
          "bss",
          Elf32_Shdr{
              .sh_name = 0,
              .sh_type = SHT_NOBITS,
              .sh_flags = SHF_ALLOC | SHF_WRITE,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // text section
  add_section(
      std::make_unique<ProgramSection>(
          "text",
          Elf32_Shdr{
              .sh_name = 0,
              .sh_type = SHT_PROGBITS,
              .sh_flags = SHF_ALLOC | SHF_EXECINSTR,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // init section
  add_section(
      std::make_unique<ProgramSection>(
          "init",
          Elf32_Shdr{
              .sh_name = 0,
              .sh_type = SHT_PROGBITS,
              .sh_flags = SHF_ALLOC | SHF_EXECINSTR,
              .sh_addr = 0,
              // TODO we have to fill this in just before writing the file
              .sh_offset = 0,
              .sh_size = 0,
              .sh_link = 0,
              .sh_info = 0,
              .sh_addralign = 0,
              .sh_entsize = 0}),
      false);

  // symbol table
  curr_symtab_idx_ = sections_.size();
  add_section(std::make_unique<SymTabSection>(
                  "symtab", Elf32_Shdr{.sh_name = 0,
                                       .sh_type = SHT_SYMTAB,
                                       .sh_flags = SHF_ALLOC,
                                       .sh_addr = 0,
                                       .sh_offset = 0,
                                       .sh_size = 0,
                                       .sh_link = 0,
                                       .sh_info = 0,
                                       .sh_addralign = 0,
                                       .sh_entsize = sizeof(Elf32_Sym)}),
              false);
}

void ELF::add_section(std::unique_ptr<ISection> section, bool relocatable) {
  // Only one section may have a given name
  auto it = std::find_if(sections_.begin(), sections_.end(), [&](auto& sec) {
    return sec->name() == section->name();
  });
  if (it != sections_.end()) {
    std::ostringstream builder{};
    builder << "Cannot add section with duplicate name: " << section->name();
    ELF_EXCEPTION(builder.str());
  }

  auto name = section->name();
  // The section header table is a special case--since it doesn't exist yet in
  // sections_, we can't get a reference to it with shStringTable(). But there's
  // more than one way to skin a cat! It must be the provided section.
  auto &shstrtab = (name == "shstrtab") ? dynamic_cast<StrTabSection&>(*section)
                                       : shstring_table();
  shstrtab.strings().push_back(name);

  sections_.push_back(std::move(section));
  // After moving the object, `*section` is no longer valid.
  if (relocatable) {
    std::ostringstream builder{};
    builder << "rel" << sections_.back()->name();
    sections_.emplace_back(std::make_unique<RelSection>(
        builder.str(),
        Elf32_Shdr{.sh_name = 0,
                   .sh_type = SHT_REL,
                   .sh_flags = 0,
                   .sh_addr = 0,
                   .sh_offset = 0,
                   .sh_size = 0,
                   .sh_link = 0,
                   .sh_info = 0,
                   .sh_addralign = 0,
                   .sh_entsize = sizeof(Elf32_Rel)},
        sections_.back()->name()));
  }
}

// void ELF::computeSectionOffsets() {
//  size_t pos = sizeof(header_) + mSectionHeaders.size() * sizeof(Elf32_Shdr);
//  for (auto pHdr = mSectionHeaders.begin(); pHdr != mSectionHeaders.end();
//       pHdr++) {
//    pHdr->sh_offset = pos;
//    pos += pHdr->sh_size;
//  }
//}

// void ELF::write(std::ostream& out) {
//  computeSectionOffsets();
//  // Write section headers
//  for (auto itHdr = mSectionHeaders.begin(); itHdr != mSectionHeaders.end();
//       itHdr++) {
//    out.write(reinterpret_cast<char*>(&(*itHdr)), sizeof(*itHdr));
//  }
//  // Write sections in the order they were initialized
//  // First .shstrtab
//  for (auto it = mSectionNames.begin(); it != mSectionNames.end(); it++) {
//    out << *it;
//  }
//  // Second .strtab
//  for (auto it = mStringTable.begin(); it != mStringTable.end(); it++) {
//    out << *it;
//  }
//  // .data
//  out.write(reinterpret_cast<char*>(mData.data()), mData.size());
//  // .rodata
//  out.write(reinterpret_cast<char*>(mRodata.data()), mRodata.size());
//  // .bss
//  out.write(reinterpret_cast<char*>(mBss.data()), mBss.size());
//  // .text
//  out.write(reinterpret_cast<char*>(mText.data()), mText.size());
//  // .init
//  out.write(reinterpret_cast<char*>(mInit.data()), mInit.size());
//  // Now symbol tables and relocation sections
//  for (size_t i = 0; i < mSymbolTables.size(); i++) {
//    // We assume the number of symbol tables and relocation sections is the
//    // same and that corresponding indices are corresponding tables
//    out.write(reinterpret_cast<char*>(&mSymbolTables.at(i)),
//              mSymbolTables.size() * sizeof(Elf32_Sym));
//    out.write(reinterpret_cast<char*>(&mRelocationSections.at(i)),
//              mRelocationSections.size() * sizeof(Elf32_Rel));
//  }
//}

ISection& ELF::set_section(const std::string& name) {
  auto it = std::find_if(sections_.begin(), sections_.end(),
                         [&](auto& sec) { return sec->name() == name; });
  if (it == sections_.end()) {
    std::ostringstream builder{};
    builder << "Invalid section: " << name;
    ELF_EXCEPTION(builder.str());
  } else {
    curr_section_ = std::distance(sections_.begin(), it);
    // If there's a corresponding relocation section, update curr_rel_idx_ also
    std::ostringstream builder{};
    builder << "rel" << name;
    auto relname = builder.str();
    auto relit =
        std::find_if(sections_.begin(), sections_.end(),
                     [&](auto& sec) { return sec->name() == relname; });
    curr_rel_idx_ = std::distance(sections_.begin(), relit);
    return *sections_.at(curr_section_);
  }
}

Elf32_Sym& ELF::add_symbol(const std::string name, uint32_t value,
                           uint32_t size, ISection::Type type,
                           ISection::Binding bind,
                           ISection::Visibility visibility, bool relocatable) {
  // TODO figure out info based on current section type
  // No other symbols in this file should have the same name
  if (symbol_names_.find(name) != symbol_names_.end()) {
    std::ostringstream builder{"Symbol "};
    builder << name;
    builder << " cannot be defined twice";
    ELF_EXCEPTION(builder.str());
  }

  symbol_names_.insert(name);

  uint32_t idx = current_symbol_table().symbols().size();
  uint32_t nameidx = string_table().strings().size();
  add_string(name);
  current_symbol_table().symbols().emplace_back(
      Elf32_Sym{.st_name = nameidx,
                .st_value = value,
                .st_size = size,
                .st_info = static_cast<uint8_t>(ELF32_ST_INFO(bind.binding(), type.type())),
                .st_other = static_cast<uint8_t>(ELF32_ST_VISIBILITY(visibility.visibility())),
                .st_shndx = static_cast<uint8_t>(curr_section_)});

  // If the symbol should be relocatable, find the corresponding section of
  // relocation information.
  if (relocatable) {
    // Search the section header table for a header of a relocation section
    // that points back to mCurrSymTab.

    // r_offset is an offset into the section where the symbol lives.
    // r_info is both the symbol's index in the symbol table and the type of
    // relocation that should occur.
    current_relocation_section().relocations().emplace_back(
        Elf32_Rel{.r_offset = value, .r_info = ELF32_R_INFO(idx, R_386_32)});
  }

  return current_symbol_table().symbols().back();
}

std::string& ELF::add_string(const std::string& str) {
  // size + null byte
  string_table().strings().push_back(str);
  string_table().header().sh_size += str.size() + 1;
  return string_table().strings().back();
}

void ELF::add_progbits(std::vector<uint8_t> data) {
  if (current_section().type() != SectionType::PROGBITS) {
    ELF_EXCEPTION("Attempted to add PROGBITS to non-PROGBITS section");
  }

  auto& section = dynamic_cast<ProgramSection&>(current_section());
  section.data().insert(section.data().end(), data.begin(), data.end());
}

void ELF::add_progbits(uint8_t* pData, size_t n) {
  if (current_section().type() != SectionType::PROGBITS) {
    ELF_EXCEPTION("Attempted to add PROGBITS to non-PROGBITS section");
  }

  auto& section = dynamic_cast<ProgramSection&>(current_section());
  section.data().insert(section.data().end(), pData, pData + n);
}
