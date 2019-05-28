
#include <string.h>
#include <string>
#include <unordered_set>
#include <vector>

#include <elf.h>

#include "parser.hpp"

namespace GBAS {
using SymbolTable = std::vector<Elf32_Sym>;
using StringTable = std::vector<std::string>;

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
using SectionHeaderTable = std::vector<Elf32_Shdr>;
using RelocationSection = std::vector<Elf32_Rel>;

/**
 * Models an ELF file, for the purposes of Game Boy programs. This means there
 * is currently no support for e.g. dynamic linking or executable files.
 */
class ELF {
 public:
  ELF();

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
   * Add str to the section names table.
   */
  std::string& addSectionName(const std::string& str);

  /**
   * Add shdr to the section headers table.
   */
  Elf32_Shdr& addSectionHeader(const Elf32_Shdr&& shdr);
  Elf32_Shdr& addSectionHeader(const Elf32_Shdr& shdr);

  /**
   * Change the current section. If a section with the given name does not
   * already exist, it will be created (its name will be added to the section
   * name string table and an entry will be added to the section header table).
   *
   * @note This function will also create a corresponding relocation section.
   *
   * @param type: valid type arguments are defined in elf.h, and are described
   * in elf(5).
   * @param addr: where this section should end up in the final executable. 0
   *   means it doesn't matter.
   *
   * @returns a reference to the header in the section header table.
   */
  Elf32_Shdr& setSection(std::string name, uint32_t addr = 0);

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
   * Helper for looking up a section's name in the section name string table
   * (mSectionNames).
   */
  std::string& getSectionName(size_t sidx);

  Elf32_Ehdr mHeader;

  /**
   * Index of the section names string table's section header in
   * mSectionHeaders.
   */
  uint32_t mSectionNamesIdx;
  StringTable mSectionNames;

  /**
   * Index of the string table's section header in mSectionHeaders.
   */
  uint32_t mStringTableIdx;
  StringTable mStringTable;

  std::vector<SymbolTable> mSymbolTables;
  std::vector<RelocationSection> mRelocationSections;
  SectionHeaderTable mSectionHeaders;
  /**
   * Index of the current symbol table's section header in mSectionHeaders.
   */
  uint16_t mCurrentSection;

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
