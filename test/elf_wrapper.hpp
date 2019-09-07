
#ifndef ELF_WRAPPER_H
#define ELF_WRAPPER_H

#include "elf.hpp"

namespace GBAS {

class ELFWrapper : public ELF {
 public:
  ELFWrapper() : ELF{} {}

  Elf32_Ehdr& getHeader() { return mHeader; }
  uint32_t getSectionNamesIdx() { return mSectionNamesIdx; }
  StringTable& getSectionNames() { return mSectionNames; }
  uint32_t getStringTableIdx() { return mStringTableIdx; }
  StringTable& getStringTable() { return mStringTable; }
  std::vector<SymbolTable>& getSymbolTables() { return mSymbolTables; }
  std::vector<RelocationSection>& getRelocationSections() {
    return mRelocationSections;
  }
  SectionHeaderTable& getSectionHeaders() { return mSectionHeaders; }
  uint16_t getCurrSymTabIdx() { return mCurrSymTab; }

  std::vector<uint8_t>& getData() { return mData; }
  uint32_t getDataIdx() { return mDataIdx; }
  std::vector<uint8_t>& getRodata() { return mRodata; }
  uint32_t getRodataIdx() { return mRodataIdx; }
  std::vector<uint8_t>& getBss() { return mBss; }
  uint32_t getBssIdx() { return mBssIdx; }
  std::vector<uint8_t>& getText() { return mText; }
  uint32_t getTextIdx() { return mTextIdx; }
  std::vector<uint8_t>& getInit() { return mInit; }
  uint32_t getInitIdx() { return mInitIdx; }

  std::unordered_set<std::string>& getSymbolNames() { return mSymbolNames; }
};

}; // namespace GBAS

#endif // ELF_WRAPPER_H
