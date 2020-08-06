
#ifndef ELF_WRAPPER_H
#define ELF_WRAPPER_H

#include <algorithm>

#include "elf.hpp"

namespace GBAS {

class ELFWrapper : public ELF {
 public:
  ELFWrapper() : ELF{} {}

  Elf32_Ehdr& get_header() { return header_; }
  uint32_t get_shstrtab_idx() { return shstrtab_idx_; }
  StrTabSection& get_shstring_table() { return shstring_table(); }
  uint32_t get_strtab_idx() { return strtab_idx_; }
  StrTabSection& get_string_table() { return string_table(); }
  SymTabSection& get_symbol_table() { return current_symbol_table(); }
  uint16_t get_curr_symtab_idx() { return curr_symtab_idx_; }
  RelSection& get_relocation_section() { return current_relocation_section(); }
  uint16_t get_curr_rel_idx() { return curr_rel_idx_; }
  SectionList& get_sections() { return sections_; }

  void add_section(std::unique_ptr<ISection> section, bool relocatable = true) {
    ELF::add_section(std::move(section), relocatable);
  }

  ISection& get_section(const std::string& name) {
    auto it = std::find_if(sections_.begin(), sections_.end(),
        [&](auto& sec) { return sec->name() == name; });
    if (it == sections_.end()) {
      std::ostringstream builder{};
      builder << "Section " << name << " not found";
      std::cerr << builder.str() << std::endl;
      throw ELFException{builder.str()};
    }
    return *(*it);
  }

  size_t get_section_idx(const std::string& name) {
    auto it = std::find_if(sections_.begin(), sections_.end(),
        [&](auto& sec) { return sec->name() == name; });
    if (it == sections_.end()) {
      std::ostringstream builder{};
      builder << "Section " << name << " not found";
      std::cerr << builder.str() << std::endl;
      throw ELFException{builder.str()};
    }
    return std::distance(sections_.begin(), it);
  }

  std::unordered_set<std::string>& get_symbol_names() { return symbol_names_; }
};

}; // namespace GBAS

#endif // ELF_WRAPPER_H
