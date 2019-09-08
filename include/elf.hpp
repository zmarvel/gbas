
#ifndef GBAS_ELF_H
#define GBAS_ELF_H

#include <string.h>
#include <string>
#include <unordered_set>
#include <vector>
#include <ostream>

#include <elf.h>

#include "parser.hpp"

namespace GBAS {

enum class SectionType {
  PROGBITS,
  STRTAB,
  SYMTAB,
  REL,
  INVALID,
};

/**
typedef struct {
  uint32_t   sh_name;
  uint32_t   sh_type;
  uint32_t   sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off  sh_offset;
  uint32_t   sh_size;
  uint32_t   sh_link;
  uint32_t   sh_info;
  uint32_t   sh_addralign;
  uint32_t   sh_entsize;
} Elf32_Shdr;
 */

class StrTabSection;

class ISection {
  public:
    ISection() : mName{}, mHeader{} { }

    ISection(std::string name, Elf32_Shdr hdr) : mName{name}, mHeader{hdr} { }

    virtual SectionType type() { return SectionType::INVALID; }

    std::string& name() { return mName; }

    Elf32_Shdr& header() { return mHeader; }

  private:
    std::string mName;
    Elf32_Shdr mHeader;
};

template <SectionType stype>
class Section : public ISection {
  public:
    Section() : ISection{} { }

    Section(std::string name, Elf32_Shdr hdr) : ISection{name, hdr} { }

    virtual SectionType type() override { return stype; }
};

class ProgramSection : public Section<SectionType::PROGBITS> {
  public:
    ProgramSection() : Section<SectionType::PROGBITS>(), mData{} { }

    ProgramSection(std::string name, Elf32_Shdr hdr)
      : Section<SectionType::PROGBITS>(name, hdr)
      , mData{}
    { }

    std::vector<uint8_t>& data() {
      return mData;
    }

  private:
    std::vector<uint8_t> mData;
};

class StrTabSection : public Section<SectionType::STRTAB> {
  public:
    using StringTable = std::vector<std::string>;

    StrTabSection() : Section<SectionType::STRTAB>(), mStrings{} { }

    StrTabSection(std::string name, Elf32_Shdr hdr)
      : Section<SectionType::STRTAB>(name, hdr)
      , mStrings{}
    { }

    StringTable& strings() {
      return mStrings;
    }

  private:
    StringTable mStrings;
};


class SymTabSection : public Section<SectionType::SYMTAB> {
  public:
    using Symbol = Elf32_Sym;
    using SymbolTable = std::vector<Symbol>;

    SymTabSection()
      : Section<SectionType::SYMTAB>()
      , mSymbols{}
    {
      mSymbols.emplace_back(
          Elf32_Sym{.st_name = 0,
          .st_value = 0,
          .st_size = 0,
          .st_info = 0,
          .st_other = 0,
          .st_shndx = 0});
    }

    SymTabSection(std::string name, Elf32_Shdr hdr)
      : Section<SectionType::SYMTAB>(name, hdr)
      , mSymbols{}
    {
      mSymbols.emplace_back(
          Elf32_Sym{.st_name = 0,
          .st_value = 0,
          .st_size = 0,
          .st_info = 0,
          .st_other = 0,
          .st_shndx = 0});
    }

    SymbolTable& symbols() {
      return mSymbols;
    }

  private:
    SymbolTable mSymbols;
};

class RelSection : public Section<SectionType::REL> {
  public:
    using Relocation = Elf32_Rel;
    using RelocationTable = std::vector<Relocation>;

    RelSection()
      : Section<SectionType::REL>()
      , mRelocations{}
    { }

    RelSection(std::string name, Elf32_Shdr hdr)
      : Section<SectionType::REL>(name, hdr)
      , mRelocations{}
    { }

    RelocationTable& relocations() {
      return mRelocations;
    }

  private:
    RelocationTable mRelocations;
};

using SectionList = std::vector<ISection>;

/**
 * Models an ELF file, for the purposes of Game Boy programs. This means there
 * is currently no support for e.g. dynamic linking or executable files.
 */
class ELF {
 public:
  ELF();

  void write(std::ostream& out);

  /**
   * Add a symbol to the current section.
   *
   * @param name: Symbol name. If there is a string table associated with the
   *   current symbol table, the name will be added to it.
   * @param value: Symbol value.
   * @param size: Size of value.
   * @param info: Type of symbol and binding properties. Values are defined in
   *   elf.h. See st_info description in elf(5).
   * @param visibility: Visibility of symbol. Values are defined in elf.h. See
   *   st_other in elf(5).
   * @param other: Defines the corresponding section for this symbol, i.e. the
   *   section where the actual data that this symbol points to is located.
   * @param relocatable: True if the symbol needs to be relocatable.
   *
   * @returns a reference to the symbol in its symbol table.
   * @throws ELFException if a symbol with that name already exists.
   */
  Elf32_Sym& addSymbol(const std::string name, uint32_t value, uint32_t size,
                       uint8_t info, uint8_t visibility, uint16_t other,
                       bool relocatable);

  /**
   * Add str to the string table.
   *
   * @returns a reference to the inserted string.
   */
  std::string& addString(const std::string& str);

  /**
   * Add some data to a PROGBITS section.
   */
  void addProgbits(std::vector<uint8_t> data);

  /**
   * Change the current section.
   *
   * @param name: Name of an already-created section.
   */
  void setSection(const std::string& name);

  /**
   * Go through each section header and compute each section's offset. If any
   * section is modified after this function has been called, the function
   * should be called again.
   */
  void computeSectionOffsets();

 protected:
  /*
     typedef struct {
     unsigned char e_ident[EI_NIDENT];
     uint16_t      e_type;
     uint16_t      e_machine;
     uint32_t      e_version;
     ElfN_Addr     e_entry;
     ElfN_Off      e_phoff;
     ElfN_Off      e_shoff;
     uint32_t      e_flags;
     uint16_t      e_ehsize;
     uint16_t      e_phentsize;
     uint16_t      e_phnum;
     uint16_t      e_shentsize;
     uint16_t      e_shnum;
     uint16_t      e_shstrndx;
     } ElfN_Ehdr;
     */

  /**
   * ELF file header.
   */
  Elf32_Ehdr mHeader;

  /**
   * Index of the section name string table in mSections.
   */
  uint32_t mShStrTabIdx;

  /**
   * Index of the current string table in mSections.
   */
  uint32_t mStrTabIdx;

  /**
   * Index of the current section in mSections.
   */
  uint32_t mCurrSection;

  /**
   * Vector of sections.
   */
  SectionList mSections;

  /**
   * Index of the current relocation section (corresponding to the current
   * progbits section) in mSections.
   */
  uint32_t mCurrRelIdx;

  /**
   * Index of current symbol table in mSections.
   */
  uint32_t mCurrSymTabIdx;

  /**
   * While this could eat up a lot of memory for a huge program, it's a lot
   * faster to check for duplicate symbol names. The alternative is linearly
   * searching through every symbol table when adding a new symbol.
   */
  std::unordered_set<std::string> mSymbolNames;
};

class ELFException : std::exception {
 public:
  ELFException(const char* msg) { mMsg = msg; }

  ELFException(const std::string& msg) { mMsg = msg; }

  virtual const char* what() const noexcept { return mMsg.c_str(); }

 private:
  std::string mMsg;
};

};  // namespace GBAS

#endif // GBAS_ELF_H
