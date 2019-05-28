

#include <algorithm>
#include <sstream>

#include "elf.hpp"

using namespace GBAS;

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
    : mSectionNames{},
      mStringTable{},
      mSymbolTables{},
      mRelocationSections{},
      mSectionHeaders{},
      mCurrentSection{0} {
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
  mSectionNamesIdx = mSectionHeaders.size();
  addSectionHeader(Elf32_Shdr{
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
      .sh_entsize = 0});
  addSectionName("shstrtab");

  // Section header for string table
  mStringTableIdx = mSectionHeaders.size();
  addSectionHeader(Elf32_Shdr{
      // This section's name will be first in the section names table
      .sh_name = 1,
      .sh_type = SHT_STRTAB,
      .sh_flags = 0,
      .sh_addr = 0,
      // TODO we have to fill this in just before writing the file
      .sh_offset = 0,
      .sh_size = 0,
      .sh_link = 0,
      .sh_info = 0,
      .sh_addralign = 0,
      .sh_entsize = 0});
  addSectionName("strtab");
}

Elf32_Sym& ELF::addSymbol(const std::string name, uint32_t value, uint32_t size,
                          uint8_t info, uint8_t visibility, uint16_t other,
                          bool relocatable) {
  Elf32_Shdr& hdr = mSectionHeaders.at(mCurrentSection);
  if (hdr.sh_type != SHT_SYMTAB) {
    throw ELFException("Attempting to define symbol in non-symbol table");
  }

  // No other symbols in this file should have the same name
  if (mSymbolNames.find(name) != mSymbolNames.end()) {
    std::ostringstream builder{"Symbol "};
    builder << name;
    builder << " cannot be defined twice";
    throw ELFException(builder.str());
  }

  auto& tbl = mSymbolTables.at(hdr.sh_offset);

  uint32_t nameIdx = mStringTable.size();
  addString(name);
  mSymbolNames.insert(name);

  uint32_t idx = tbl.size();
  tbl.emplace_back(Elf32_Sym{.st_name = nameIdx,
                             .st_value = value,
                             .st_size = size,
                             .st_info = info,
                             .st_other = visibility,
                             .st_shndx = other});

  // If the symbol should be relocatable, find the corresponding section of
  // relocation information.
  if (relocatable) {
    // Search the section header table for a header of a relocation section
    // that points back to mCurrentSection.
    auto relocHdr =
        std::find_if(mSectionHeaders.begin(), mSectionHeaders.end(),
                     [&](auto h) { return h.sh_link == mCurrentSection; });
    if (relocHdr == mSectionHeaders.end()) {
      std::ostringstream builder{"Relocatable symbol "};
      builder << name << " requested but no corresponding relocation"
              << " section found";
      throw ELFException(builder.str());
    }

    auto relocEntry = mRelocationSections.at(relocHdr->sh_offset);
    // r_offset is the offset into the symbol table of the symbol we just added.
    // r_info is both the symbol's index in the symbol table and the type of
    // relocation that should occur.
    relocEntry.emplace_back(
        Elf32_Rel{.r_offset = static_cast<Elf32_Addr>(idx * sizeof(Elf32_Sym)),
                  .r_info = ELF32_R_SYM(idx) | ELF32_R_TYPE(R_386_32)});
  }

  return tbl.back();
}

std::string& ELF::addString(const std::string& str) {
  Elf32_Shdr& hdr = mSectionHeaders.at(mStringTableIdx);
  hdr.sh_size += 1;
  mStringTable.push_back(str);
  return mStringTable.back();
}

std::string& ELF::addSectionName(const std::string& str) {
  Elf32_Shdr& hdr = mSectionHeaders.at(mSectionNamesIdx);
  hdr.sh_size += 1;
  mSectionNames.push_back(str);
  return mSectionNames.back();
}

Elf32_Shdr& ELF::addSectionHeader(const Elf32_Shdr&& shdr) {
  mSectionHeaders.emplace_back(shdr);
  mHeader.e_shnum += 1;
  return mSectionHeaders.back();
}

Elf32_Shdr& ELF::addSectionHeader(const Elf32_Shdr& shdr) {
  mSectionHeaders.push_back(shdr);
  mHeader.e_shnum += 1;
  return mSectionHeaders.back();
}

Elf32_Shdr& ELF::setSection(std::string name, uint32_t addr) {
  // TODO only allow setting section to symbol tables. I think we can handle
  // relocation and string tables under the hood.
  for (size_t i = 0; i < mSectionHeaders.size(); i++) {
    if (name == getSectionName(i)) {
      if (mSectionHeaders.at(i).sh_type != SHT_SYMTAB) {
        std::ostringstream builder{"Attempted to change to invalid section "};
        builder << name;
        throw ELFException(builder.str());
      } else {
        mCurrentSection = i;
        return mSectionHeaders.at(i);
      }
    }
  }
  // If we get here, we didn't find an existing section, so we'll create one.
  uint32_t nameidx = static_cast<uint32_t>(mSectionNames.size());
  mSectionNames.push_back(name);
  // TODO while we're building the ELF file, we'll cheat a little bit and use
  // this as an index into mSymbolTables.
  uint32_t offset = mSymbolTables.size();
  mSymbolTables.emplace_back(SymbolTable{});
  // The first entry in each symbol table is a null entry
  mSymbolTables.at(offset).emplace_back(Elf32_Sym{.st_name = 0,
                                                  .st_value = 0,
                                                  .st_size = 0,
                                                  .st_info = 0,
                                                  .st_other = 0,
                                                  .st_shndx = 0});
  uint32_t relOffset = mRelocationSections.size();
  mRelocationSections.emplace_back(RelocationSection{});
  uint32_t symtabLink = mSectionHeaders.size();
  addSectionHeader(
      Elf32_Shdr{.sh_name = nameidx,
                 .sh_type = SHT_SYMTAB,
                 // These are just symbols. .text, .rodata, and .data have
                 // a non-zero sh_flags field.
                 .sh_flags = 0,
                 .sh_addr = addr,
                 .sh_offset = offset,
                 // Size includes the null entry at the beginning of the table.
                 // Make sure to update this field when adding a symbol!
                 .sh_size = 1,
                 // For a symbol table, link is the index in the section header
                 // table of the corresponding string table's section header.
                 .sh_link = mStringTableIdx,
                 .sh_info = 0,
                 // This indicates that the section has aligment constraints. 0
                 // or 1 mean the same thing. TODO is there a problem with
                 // aligning everything to 2 bytes?
                 .sh_addralign = 0,
                 .sh_entsize = sizeof(Elf32_Sym)});

  uint32_t relnameidx = mSectionNames.size();
  addSectionName("rel" + name);
  addSectionHeader(Elf32_Shdr{
      .sh_name = relnameidx,
      .sh_type = SHT_REL,
      .sh_flags = 0,
      .sh_addr = 0,
      .sh_offset = relOffset,
      // TODO is there a null entry at the beginning?
      .sh_size = 0,
      // For a relocation section, link is the index in the section header table
      // of the corresponding symbol table's section header.
      .sh_link = symtabLink,
      // Confusingly, for a relocation section, this is the same as sh_link
      // according to https://wiki.osdev.org/ELF_Tutorial#Relocation_Sections.
      // TODO does any other section type use this field??
      .sh_info = symtabLink,
      .sh_addralign = 0,
      .sh_entsize = sizeof(Elf32_Rel)});

  mCurrentSection = symtabLink;

  return mSectionHeaders.at(symtabLink);
}

std::string& ELF::getSectionName(size_t sidx) {
  auto& sh = mSectionHeaders.at(sidx);
  return mSectionNames.at(sh.sh_name);
}

void ELF::computeSectionOffsets() {
  // TODO
}
