

#include <algorithm>
#include <sstream>
#include <string_view>

#include "elf.hpp"

using namespace GBAS;

#undef DEBUG_ELF
#ifndef DEBUG_GBAS
#define DEBUG_ELF
#else
#define DEBUG_ELF
#endif

#ifdef DEBUG_ELF
#define ELF_EXCEPTION(msg) \
  do {\
    std::cerr << msg << std::endl; \
    throw ELFException{msg}; \
  } while(0)
#else
#define ELF_EXCEPTION(msg) \
  do {\
    throw ELFException{msg}; \
  } while(0)
#endif

static const int ABIVERSION = 0;
static const unsigned char ELF_IDENT[] = {
    EI_MAG0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
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

ELF::ELF()
  : mCurrSection{0}
  , mCurrRelIdx{0}
{
  // File header
  memcpy(&mHeader.e_ident, ELF_IDENT, EI_NIDENT);
  mHeader.e_type = ET_REL;
  mHeader.e_machine = EM_NONE;
  mHeader.e_version = EV_CURRENT;
  mHeader.e_entry = 0;
  mHeader.e_phoff = 0;
  mHeader.e_shoff = 0;  // set this later
  mHeader.e_flags = 0;
  mHeader.e_ehsize = sizeof(Elf32_Ehdr);
  mHeader.e_phentsize = sizeof(Elf32_Phdr);
  mHeader.e_phnum = 0;
  mHeader.e_shentsize = sizeof(Elf32_Shdr);
  mHeader.e_shnum = 0;  // set this later

  // Section header for section names string table
  mShStrTabIdx = mSections.size();
  addSection(std::make_unique<StrTabSection>("shstrtab", Elf32_Shdr{
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
      .sh_entsize = 0}), false);

  // Section header for string table
  mStrTabIdx = mSections.size();
  addSection(std::make_unique<StrTabSection>("strtab", Elf32_Shdr{
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
      .sh_entsize = 0}), false);

  // data section
  addSection(std::make_unique<ProgramSection>("data",
      Elf32_Shdr{.sh_name = 0,
      .sh_type = SHT_PROGBITS,
      .sh_flags = SHF_ALLOC | SHF_WRITE,
      .sh_addr = 0,
      // TODO we have to fill this in just before writing the file
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = 0}));


  // rodata section
  addSection(std::make_unique<ProgramSection>("rodata",
      Elf32_Shdr{.sh_name = 0,
      .sh_type = SHT_PROGBITS,
      .sh_flags = SHF_ALLOC,
      .sh_addr = 0,
      // TODO we have to fill this in just before writing the file
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = 0}));

  // bss section
  addSection(std::make_unique<ProgramSection>("bss",
      Elf32_Shdr{.sh_name = 0,
      .sh_type = SHT_NOBITS,
      .sh_flags = SHF_ALLOC | SHF_WRITE,
      .sh_addr = 0,
      // TODO we have to fill this in just before writing the file
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = 0}));

  // text section
  addSection(std::make_unique<ProgramSection>("text",
      Elf32_Shdr{.sh_name = 0,
      .sh_type = SHT_PROGBITS,
      .sh_flags = SHF_ALLOC | SHF_EXECINSTR,
      .sh_addr = 0,
      // TODO we have to fill this in just before writing the file
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = 0}));

  // init section
  addSection(std::make_unique<ProgramSection>("init", 
      Elf32_Shdr{.sh_name = 0,
      .sh_type = SHT_PROGBITS,
      .sh_flags = SHF_ALLOC | SHF_EXECINSTR,
      .sh_addr = 0,
      // TODO we have to fill this in just before writing the file
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = 0}));

  // symbol table
  mCurrSymTabIdx = mSections.size();
  addSection(std::make_unique<SymTabSection>("symtab",
      Elf32_Shdr{.sh_name = 0,
      .sh_type = SHT_SYMTAB,
      .sh_flags = SHF_ALLOC,
      .sh_addr = 0,
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = sizeof(Elf32_Sym)}), false);
}

void ELF::addSection(std::unique_ptr<ISection>&& section, bool relocatable) {
  // Only one section may have a given name
  auto it = std::find_if(mSections.begin(), mSections.end(),
      [&](auto& sec) { return sec->name() == section->name(); });
  if (it != mSections.end()) {
    std::ostringstream builder{};
    builder << "Cannot add section with duplicate name: " << section->name();
    ELF_EXCEPTION(builder.str());
  }

  mSections.emplace_back(std::move(section));
  if (relocatable) {
    std::ostringstream builder{};
    builder << "rel" << mSections.back()->name();
    mSections.emplace_back(std::make_unique<RelSection>(
          builder.str(),
          Elf32_Shdr{
          .sh_name = 0,
          .sh_type = SHT_REL,
          .sh_flags = 0,
          .sh_addr = 0,
          .sh_offset = 0,
          .sh_size = 0,
          .sh_link = 0,
          .sh_info = 0,
          .sh_addralign = 0,
          .sh_entsize = sizeof(Elf32_Rel)},
          mSections.back()->name()));
  }
}

//void ELF::computeSectionOffsets() {
//  size_t pos = sizeof(mHeader) + mSectionHeaders.size() * sizeof(Elf32_Shdr);
//  for (auto pHdr = mSectionHeaders.begin(); pHdr != mSectionHeaders.end();
//       pHdr++) {
//    pHdr->sh_offset = pos;
//    pos += pHdr->sh_size;
//  }
//}

//void ELF::write(std::ostream& out) {
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

ISection& ELF::setSection(const std::string& name) {
  auto it = std::find_if(mSections.begin(), mSections.end(),
                         [&](auto& sec) { return sec->name() == name; });
  if (it == mSections.end()) {
    std::ostringstream builder{};
    builder << "Invalid section: " << name;
    ELF_EXCEPTION(builder.str());
  } else {
    mCurrSection = std::distance(mSections.begin(), it);
    return *mSections.at(mCurrSection);
  }
}

Elf32_Sym& ELF::addSymbol(const std::string name, uint32_t value, uint32_t size,
    ISection::Type type, ISection::Binding bind,
    ISection::Visibility visibility, bool relocatable) {
  // TODO figure out info based on current section type
  // No other symbols in this file should have the same name
  if (mSymbolNames.find(name) != mSymbolNames.end()) {
    std::ostringstream builder{"Symbol "};
    builder << name;
    builder << " cannot be defined twice";
    ELF_EXCEPTION(builder.str());
  }

  mSymbolNames.insert(name);

  uint32_t idx = currentSymbolTable().symbols().size();
  uint32_t nameIdx = stringTable().strings().size();
  addString(name);
  currentSymbolTable().symbols().emplace_back(Elf32_Sym{.st_name = nameIdx,
      .st_value = value,
      .st_size = size,
      .st_info = ELF32_ST_INFO(bind.binding(), type.type()),
      .st_other = ELF32_ST_VISIBILITY(visibility.visibility()),
      .st_shndx = static_cast<uint8_t>(mCurrSection)});

  // If the symbol should be relocatable, find the corresponding section of
  // relocation information.
  if (relocatable) {
    // Search the section header table for a header of a relocation section
    // that points back to mCurrSymTab.

    // r_offset is the offset into the symbol table of the symbol we just added.
    // r_info is both the symbol's index in the symbol table and the type of
    // relocation that should occur.
    currentRelocationSection().relocations().emplace_back(
        Elf32_Rel{.r_offset = static_cast<Elf32_Addr>(idx * sizeof(Elf32_Sym)),
        .r_info = ELF32_R_SYM(idx) | ELF32_R_TYPE(R_386_32)});
  }

  return currentSymbolTable().symbols().back();
}

std::string& ELF::addString(const std::string& str) {
  // size + null byte
  stringTable().strings().push_back(str);
  stringTable().header().sh_size += str.size() + 1;
  return stringTable().strings().back();
}

void ELF::addProgbits(std::vector<uint8_t> data) {
  if (currentSection().type() != SectionType::PROGBITS) {
    ELF_EXCEPTION("Attempted to add PROGBITS to non-PROGBITS section");
  }

  auto& section = dynamic_cast<ProgramSection&>(currentSection());
  section.data().insert(section.data().end(), data.begin(), data.end());
}

void ELF::addProgbits(uint8_t* pData, size_t n) {
  if (currentSection().type() != SectionType::PROGBITS) {
    ELF_EXCEPTION("Attempted to add PROGBITS to non-PROGBITS section");
  }

  auto& section = dynamic_cast<ProgramSection&>(currentSection());
  section.data().insert(section.data().end(), pData, pData + n);
}
