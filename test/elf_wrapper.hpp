
#ifndef ELF_WRAPPER_H
#define ELF_WRAPPER_H

#include <algorithm>

#include "elf.hpp"

namespace GBAS {

class ELFWrapper : public ELF {
 public:
  ELFWrapper() : ELF{} {}

  Elf32_Ehdr& getHeader() { return mHeader; }
  uint32_t getShStrTabIdx() { return mShStrTabIdx; }
  StrTabSection& getShStringTable() { return shStringTable(); }
  uint32_t getStrTabIdx() { return mStrTabIdx; }
  StrTabSection& getStringTable() { return stringTable(); }
  SymTabSection& getSymbolTable() { return currentSymbolTable(); }
  uint16_t getCurrSymTabIdx() { return mCurrSymTabIdx; }
  RelSection& getRelocationSection() { return currentRelocationSection(); }
  uint16_t getCurrRelIdx() { return mCurrRelIdx; }
  SectionList& getSections() { return mSections; }

  void addSection(std::unique_ptr<ISection>&& section, bool relocatable = true) {
    ELF::addSection(std::move(section), relocatable);
  }

  ISection& getSection(const std::string& name) {
    auto it = std::find_if(mSections.begin(), mSections.end(),
        [&](auto& sec) { return sec->name() == name; });
    if (it == mSections.end()) {
      std::ostringstream builder{};
      builder << "Section " << name << " not found";
      std::cerr << builder.str() << std::endl;
      throw ELFException{builder.str()};
    }
    return *(*it);
  }

  size_t getSectionIdx(const std::string& name) {
    auto it = std::find_if(mSections.begin(), mSections.end(),
        [&](auto& sec) { return sec->name() == name; });
    if (it == mSections.end()) {
      std::ostringstream builder{};
      builder << "Section " << name << " not found";
      std::cerr << builder.str() << std::endl;
      throw ELFException{builder.str()};
    }
    return std::distance(mSections.begin(), it);
  }

  std::unordered_set<std::string>& getSymbolNames() { return mSymbolNames; }
};

}; // namespace GBAS

#endif // ELF_WRAPPER_H
