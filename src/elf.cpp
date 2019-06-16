

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
      mCurrSymTab{0},
      mData{},
      mDataIdx{0},
      mRodata{},
      mRodataIdx{0},
      mBss{},
      mBssIdx{0},
      mText{},
      mTextIdx{0},
      mInit{},
      mInitIdx{0} {
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

  // data section
  mDataIdx = mSectionHeaders.size();
  addSectionHeader(
      Elf32_Shdr{.sh_name = 2,
                 .sh_type = SHT_PROGBITS,
                 .sh_flags = SHF_ALLOC | SHF_WRITE,
                 .sh_addr = 0,
                 // TODO we have to fill this in just before writing the file
                 .sh_offset = 0,
                 .sh_size = 0,
                 .sh_link = 0,
                 .sh_info = 0,
                 .sh_addralign = 0,
                 .sh_entsize = 0});
  addSectionName("data");

  // rodata section
  mRodataIdx = mSectionHeaders.size();
  addSectionHeader(
      Elf32_Shdr{.sh_name = 3,
                 .sh_type = SHT_PROGBITS,
                 .sh_flags = SHF_ALLOC,
                 .sh_addr = 0,
                 // TODO we have to fill this in just before writing the file
                 .sh_offset = 0,
                 .sh_size = 0,
                 .sh_link = 0,
                 .sh_info = 0,
                 .sh_addralign = 0,
                 .sh_entsize = 0});
  addSectionName("rodata");

  // bss section
  mBssIdx = mSectionHeaders.size();
  addSectionHeader(
      Elf32_Shdr{.sh_name = 4,
                 .sh_type = SHT_NOBITS,
                 .sh_flags = SHF_ALLOC | SHF_WRITE,
                 .sh_addr = 0,
                 // TODO we have to fill this in just before writing the file
                 .sh_offset = 0,
                 .sh_size = 0,
                 .sh_link = 0,
                 .sh_info = 0,
                 .sh_addralign = 0,
                 .sh_entsize = 0});
  addSectionName("bss");

  // text section
  mTextIdx = mSectionHeaders.size();
  addSectionHeader(
      Elf32_Shdr{.sh_name = 5,
                 .sh_type = SHT_PROGBITS,
                 .sh_flags = SHF_ALLOC | SHF_EXECINSTR,
                 .sh_addr = 0,
                 // TODO we have to fill this in just before writing the file
                 .sh_offset = 0,
                 .sh_size = 0,
                 .sh_link = 0,
                 .sh_info = 0,
                 .sh_addralign = 0,
                 .sh_entsize = 0});
  addSectionName("text");

  // init section
  mInitIdx = mSectionHeaders.size();
  addSectionHeader(
      Elf32_Shdr{.sh_name = 6,
                 .sh_type = SHT_PROGBITS,
                 .sh_flags = SHF_ALLOC | SHF_EXECINSTR,
                 .sh_addr = 0,
                 // TODO we have to fill this in just before writing the file
                 .sh_offset = 0,
                 .sh_size = 0,
                 .sh_link = 0,
                 .sh_info = 0,
                 .sh_addralign = 0,
                 .sh_entsize = 0});
  addSectionName("init");
}

void ELF::computeSectionOffsets() {
  size_t pos = sizeof(mHeader) + mSectionHeaders.size() * sizeof(Elf32_Shdr);
  for (auto pHdr = mSectionHeaders.begin(); pHdr != mSectionHeaders.end();
       pHdr++) {
    pHdr->sh_offset = pos;
    pos += pHdr->sh_size;
  }
}

void ELF::write(std::ostream& out) {
  computeSectionOffsets();
  // Write section headers
  for (auto itHdr = mSectionHeaders.begin(); itHdr != mSectionHeaders.end();
       itHdr++) {
    out.write(reinterpret_cast<char*>(&(*itHdr)), sizeof(*itHdr));
  }
  // Write sections in the order they were initialized
  // First .shstrtab
  for (auto it = mSectionNames.begin(); it != mSectionNames.end(); it++) {
    out << *it;
  }
  // Second .strtab
  for (auto it = mStringTable.begin(); it != mStringTable.end(); it++) {
    out << *it;
  }
  // .data
  out.write(reinterpret_cast<char*>(mData.data()), mData.size());
  // .rodata
  out.write(reinterpret_cast<char*>(mRodata.data()), mRodata.size());
  // .bss
  out.write(reinterpret_cast<char*>(mBss.data()), mBss.size());
  // .text
  out.write(reinterpret_cast<char*>(mText.data()), mText.size());
  // .init
  out.write(reinterpret_cast<char*>(mInit.data()), mInit.size());
  // Now symbol tables and relocation sections
  for (size_t i = 0; i < mSymbolTables.size(); i++) {
    // We assume the number of symbol tables and relocation sections is the
    // same and that corresponding indices are corresponding tables
    out.write(reinterpret_cast<char*>(&mSymbolTables.at(i)),
              mSymbolTables.size() * sizeof(Elf32_Sym));
    out.write(reinterpret_cast<char*>(&mRelocationSections.at(i)),
              mRelocationSections.size() * sizeof(Elf32_Rel));
  }
}

Elf32_Sym& ELF::addSymbol(const std::string name, uint32_t value, uint32_t size,
                          uint8_t info, uint8_t visibility, uint16_t other,
                          bool relocatable) {
  Elf32_Shdr& hdr = mSectionHeaders.at(mCurrSymTab);
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
  hdr.sh_size += sizeof(Elf32_Sym);

  // If the symbol should be relocatable, find the corresponding section of
  // relocation information.
  if (relocatable) {
    // Search the section header table for a header of a relocation section
    // that points back to mCurrSymTab.
    auto relocHdr =
        std::find_if(mSectionHeaders.begin(), mSectionHeaders.end(),
                     [&](auto h) { return h.sh_link == mCurrSymTab; });
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
    relocHdr->sh_size += sizeof(Elf32_Rel);
  }

  return tbl.back();
}

std::string& ELF::addString(const std::string& str) {
  Elf32_Shdr& hdr = mSectionHeaders.at(mStringTableIdx);
  // size + null byte
  hdr.sh_size += str.size() + 1;
  mStringTable.push_back(str);
  return mStringTable.back();
}

std::string& ELF::addSectionName(const std::string& str) {
  Elf32_Shdr& hdr = mSectionHeaders.at(mSectionNamesIdx);
  // size + null byte
  hdr.sh_size += str.size() + 1;
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

size_t ELF::addData(const std::vector<uint8_t>& data) {
  size_t start = mData.capacity();
  mData.resize(mData.capacity() + data.size());
  for (size_t i = 0; i < data.size(); i++) {
    mData[start + i] = data[i];
  }
  mSectionHeaders.at(mDataIdx).sh_size += data.size();
  return data.size();
}

size_t ELF::dataSize() {
  return mData.size();
}

size_t ELF::addRodata(const std::vector<uint8_t>& data) {
  size_t start = mRodata.capacity();
  mRodata.resize(mRodata.capacity() + data.size());
  for (size_t i = 0; i < data.size(); i++) {
    mRodata[start + i] = data[i];
  }
  mSectionHeaders.at(mRodataIdx).sh_size += data.size();
  return data.size();
}

size_t ELF::rodataSize() {
  return mRodata.size();
}

size_t ELF::addBss(const std::vector<uint8_t>& data) {
  size_t start = mBss.capacity();
  mBss.resize(mBss.capacity() + data.size());
  for (size_t i = 0; i < data.size(); i++) {
    mBss[start + i] = data[i];
  }
  mSectionHeaders.at(mBssIdx).sh_size += data.size();
  return data.size();
}

size_t ELF::bssSize() {
  return mBss.size();
}

size_t ELF::addText(const std::vector<uint8_t>& data) {
  size_t start = mText.capacity();
  mText.resize(mText.capacity() + data.size());
  for (size_t i = 0; i < data.size(); i++) {
    mText[start + i] = data[i];
  }
  mSectionHeaders.at(mTextIdx).sh_size += data.size();
  return data.size();
}

size_t ELF::textSize() {
  return mText.size();
}

size_t ELF::addInit(const std::vector<uint8_t>& data) {
  size_t start = mInit.capacity();
  mInit.resize(mInit.capacity() + data.size());
  for (size_t i = 0; i < data.size(); i++) {
    mInit[start + i] = data[i];
  }
  mSectionHeaders.at(mInitIdx).sh_size += data.size();
  return data.size();
}

size_t ELF::initSize() {
  return mInit.size();
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
        mCurrSymTab = i;
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
                 .sh_size = sizeof(Elf32_Sym),
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

  mCurrSymTab = symtabLink;

  return mSectionHeaders.at(symtabLink);
}

std::string& ELF::getSectionName(size_t sidx) {
  auto& sh = mSectionHeaders.at(sidx);
  return mSectionNames.at(sh.sh_name);
}
