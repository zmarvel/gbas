
#ifndef GBAS_ELF_H
#define GBAS_ELF_H

#include <string.h>
#include <string>
#include <unordered_set>
#include <vector>
#include <ostream>
#include <numeric>

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
    ISection() : mName{}, header_{} { }

    ISection(std::string name, Elf32_Shdr hdr) : mName{name}, header_{hdr} { }

    virtual SectionType type() { return SectionType::INVALID; }

    std::string& name() { return mName; }

    Elf32_Shdr& header() { return header_; }

    virtual size_t size() const = 0;

    virtual void write(std::ostream& os) const = 0;

    class Type {
      public:
        // no type by default
        Type() : mType{STT_NOTYPE} { }

        Type noType() { return Type{STT_NOTYPE}; }
        Type object() { return Type{STT_OBJECT}; }
        Type function() { return Type{STT_FUNC}; }
        Type section() { return Type{STT_SECTION}; }
        Type file() { return Type{STT_FILE}; }

        uint8_t type() {
          return mType;
        }

      private:
        Type(uint8_t type) : mType{type} { }

        uint8_t mType;
    };

    class Binding {
      public:
        // global by default
        Binding() : mBinding{STB_GLOBAL} { }

        Binding local() { return Binding{STB_LOCAL}; }
        Binding global() { return Binding{STB_GLOBAL}; }
        Binding weak() { return Binding{STB_WEAK}; }

        uint8_t binding() { return mBinding; }

      private:
        Binding(uint8_t type) : mBinding{type} { }

        uint8_t mBinding;
    };

    class Visibility {
      public:
        // default visibility
        Visibility() : mVisibility{STV_DEFAULT} { }

        Visibility default_() { return Visibility{STV_DEFAULT}; }
        Visibility internal() { return Visibility{STV_INTERNAL}; }
        Visibility hidden() { return Visibility{STV_HIDDEN}; }
        Visibility protected_() { return Visibility{STV_PROTECTED}; }

        uint8_t visibility() { return mVisibility; }

      private:
        Visibility(uint8_t type) : mVisibility{type} { }

        uint8_t mVisibility;
    };


  private:
    std::string mName;
    Elf32_Shdr header_;
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

    virtual size_t size() const override { return data().size(); }

    const std::vector<uint8_t>& data() const {
      return mData;
    }


    std::vector<uint8_t>& data() {
      return mData;
    }

    virtual void write(std::ostream& os) const override {
      os.write(reinterpret_cast<const char*>(mData.data()), mData.size());
    }

    void append(std::vector<uint8_t>& buf) {
      mData.insert(mData.end(), buf.begin(), buf.end());
      header().sh_size += buf.size();
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

    virtual size_t size() const override {
      // TODO maybe we should add 1 to the length of every string to account for
      // the terminating byte
      return std::accumulate(strings().begin(),
                             strings().end(),
                             0,
                             [](size_t total_len, std::string st) -> size_t {
                               return total_len + st.size();
                             });
    }

    const StringTable& strings() const {
      return mStrings;
    }


    StringTable& strings() {
      return mStrings;
    }

    virtual void write(std::ostream& os) const override {
      for (auto it = strings().begin(); it != strings().end(); it++) {
        os.write(it->c_str(), it->size());
        os.write("\0", 1);
      }
    }

  private:
    StringTable mStrings;
};

class SymTabSection : public Section<SectionType::SYMTAB> {
 public:
  using Symbol = Elf32_Sym;
  using SymbolTable = std::vector<Symbol>;

  SymTabSection() : Section<SectionType::SYMTAB>(), mSymbols{} {
    // First entry is always full of zeros.
    mSymbols.emplace_back(Elf32_Sym{.st_name = 0,
                                    .st_value = 0,
                                    .st_size = 0,
                                    .st_info = 0,
                                    .st_other = 0,
                                    .st_shndx = 0});
  }

  SymTabSection(std::string name, Elf32_Shdr hdr)
      : Section<SectionType::SYMTAB>(name, hdr), mSymbols{} {
    mSymbols.emplace_back(Elf32_Sym{.st_name = 0,
                                    .st_value = 0,
                                    .st_size = 0,
                                    .st_info = 0,
                                    .st_other = 0,
                                    .st_shndx = 0});
  }

  virtual size_t size() const override {
    // TODO maybe we should add 1 to the length of every string to account for
    // the terminating byte
    return sizeof(Symbol) * symbols().size();
  }

  const SymbolTable& symbols() const { return mSymbols; }

  SymbolTable& symbols() { return mSymbols; }

  virtual void write(std::ostream& os) const override {
    for (auto it = symbols().begin(); it != symbols().end(); it++) {
      auto hdr = *it;
      os.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    }
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

    RelSection(std::string name, Elf32_Shdr hdr, std::string other)
      : Section<SectionType::REL>(name, hdr)
      , mOther{other}
      , mRelocations{}
    { }

    virtual size_t size() const override {
      return sizeof(Relocation) * mRelocations.size();
    }

    const std::string& other() { return mOther; }

    RelocationTable& relocations() {
      return mRelocations;
    }

    const RelocationTable& relocations() const {
      return mRelocations;
    }

    virtual void write(std::ostream& os) const override {
      // TODO is the first entry supposed to be null?
      for (auto it = relocations().begin(); it != relocations().end(); it++) {
        os.write(reinterpret_cast<const char*>(&(*it)), sizeof(*it));
      }
    }

  private:
    std::string mOther;
    RelocationTable mRelocations;
};

using SectionList = std::vector<std::unique_ptr<ISection>>;



template <typename headerT>
headerT swap_elf_header(headerT& hdr);

template <>
Elf32_Ehdr GBAS::swap_elf_header(Elf32_Ehdr& hdr);

template <typename sheaderT>
sheaderT swap_section_header(sheaderT& hdr);

template <>
Elf32_Shdr GBAS::swap_section_header(Elf32_Shdr& hdr);


/**
 * Models an ELF file, for the purposes of Game Boy programs. This means there
 * is currently no support for e.g. dynamic linking or executable files.
 */
class ELF
{
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
   * @param other: Defines the corresponding section for this symbol, i.e. the
   *   section where the actual data that this symbol points to is located.
   * @param relocatable: True if the symbol needs to be relocatable.
   *
   * @returns a reference to the symbol in its symbol table.
   * @throws ELFException if a symbol with that name already exists.
   */
  Elf32_Sym& add_symbol(const std::string name, uint32_t value, uint32_t size,
                        ISection::Type type, ISection::Binding bind,
                        ISection::Visibility visibility, bool relocatable);

  /**
   * Add str to the string table.
   *
   * @returns a reference to the inserted string.
   */
  std::string& add_string(const std::string& str);

  /**
   * Add some data to a PROGBITS section.
   */
  void add_progbits(std::vector<uint8_t> data);

  /**
   * Add some data to a PROGBITS section.
   *
   * @param pData: Pointer to bytes that will be copied.
   * @param n: How many bytes will be copied.
   */
  void add_progbits(uint8_t* pData, size_t n);

  /**
   * Change the current section.
   *
   * @param name: Name of an already-created section.
   */
  ISection& set_section(const std::string& name);

  /**
   * Go through each section header and compute each section's offset. If any
   * section is modified after this function has been called, the function
   * should be called again.
   */
  void comput_section_offsets();

  Elf32_Ehdr& header() { return header_; }

  const SectionList& sections() { return sections_; }

protected:
  /**
   * Add a section. Only in-place construction supported.
   *
   * @param section: rvalue reference to ISection unique_ptr.
   * @param relocatable: Should a corresponding RelocationSection be created?
   */
  void add_section(std::unique_ptr<ISection>&& section, bool relocatable = true);

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
  Elf32_Ehdr header_;

  /**
   * Vector of sections.
   */
  SectionList sections_;

  /**
   * Index of the section name string table in sections_.
   */
  uint32_t shstrtab_idx_;

public:
  StrTabSection& shStringTable()
  {
    return dynamic_cast<StrTabSection&>(*sections_.at(shstrtab_idx_));
  }

protected:
  /**
   * Index of the current string table in sections_.
   */
  uint32_t strtab_idx_;

  StrTabSection& string_table()
  {
    return dynamic_cast<StrTabSection&>(*sections_.at(strtab_idx_));
  }

  /**
   * Index of the current section in sections_.
   */
  uint32_t curr_section_;

  ISection& current_section() { return *sections_.at(curr_section_); }

  /**
   * Index of the current relocation section (corresponding to the current
   * progbits section) in sections_.
   */
  uint32_t curr_rel_idx_;

  RelSection& current_relocation_section()
  {
    return dynamic_cast<RelSection&>(*sections_.at(curr_rel_idx_));
  }

  /**
   * Index of current symbol table in sections_.
   */
  uint32_t curr_symtab_idx_;

  SymTabSection& current_symbol_table()
  {
    return dynamic_cast<SymTabSection&>(*sections_.at(curr_symtab_idx_));
  }

  /**
   * While this could eat up a lot of memory for a huge program, it's a lot
   * faster to check for duplicate symbol names. The alternative is linearly
   * searching through every symbol table when adding a new symbol.
   */
  std::unordered_set<std::string> symbol_names_;
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
