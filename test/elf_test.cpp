
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "elf.hpp"

using namespace GBAS;

BOOST_AUTO_TEST_SUITE(elf);

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
  uint16_t getCurrentSectionIdx() { return mCurrentSection; }
  std::unordered_set<std::string>& getSymbolNames() { return mSymbolNames; }
};

BOOST_AUTO_TEST_CASE(elf_test_constructor) {
  ELFWrapper elf{};

  // Test the file header
  Elf32_Ehdr& elfHeader = elf.getHeader();

  BOOST_CHECK_EQUAL(elfHeader.e_ident[0], EI_MAG0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[1], EI_MAG1);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[2], EI_MAG2);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[3], EI_MAG3);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[4], ELFCLASS32);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[5], ELFDATA2MSB);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[6], EV_CURRENT);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[7], ELFOSABI_STANDALONE);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[8], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[9], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[10], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[11], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[12], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[13], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[14], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[15], 0);
  BOOST_CHECK_EQUAL(elfHeader.e_type, ET_REL);
  BOOST_CHECK_EQUAL(elfHeader.e_machine, EM_NONE);
  BOOST_CHECK_EQUAL(elfHeader.e_version, EV_CURRENT);
  BOOST_CHECK_EQUAL(elfHeader.e_entry, 0);
  BOOST_CHECK_EQUAL(elfHeader.e_phoff, 0);
  BOOST_CHECK_EQUAL(elfHeader.e_shoff, 0);
  BOOST_CHECK_EQUAL(elfHeader.e_flags, 0);
  BOOST_CHECK_EQUAL(elfHeader.e_ehsize, sizeof(Elf32_Ehdr));
  BOOST_CHECK_EQUAL(elfHeader.e_phentsize, sizeof(Elf32_Phdr));
  BOOST_CHECK_EQUAL(elfHeader.e_phnum, 0);
  BOOST_CHECK_EQUAL(elfHeader.e_shentsize, sizeof(Elf32_Shdr));
  // This is 0 in the beginning of the constructor, but should be 2 after
  // adding the two string tables.
  // TODO and eventuall rodata, etc.
  BOOST_CHECK_EQUAL(elfHeader.e_shnum, 2);

  StringTable& sectionNames = elf.getSectionNames();
  SectionHeaderTable& sectionHeaders = elf.getSectionHeaders();

  // Test the section name string table .shstrtab
  {
    BOOST_CHECK_EQUAL(sectionNames.at(0), "shstrtab");
    Elf32_Shdr& hdr = sectionHeaders.at(0);
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_STRTAB);
    BOOST_CHECK_EQUAL(hdr.sh_flags, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    // This is initially zero, but the constructor adds two section names
    BOOST_CHECK_EQUAL(hdr.sh_size, 2);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the string table .strtab
  {
    BOOST_CHECK_EQUAL(sectionNames.at(1), "strtab");
    Elf32_Shdr& hdr = sectionHeaders.at(1);
    BOOST_CHECK_EQUAL(hdr.sh_name, 1);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_STRTAB);
    BOOST_CHECK_EQUAL(hdr.sh_flags, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }
}

BOOST_AUTO_TEST_CASE(elf_test_addString) {
  ELFWrapper elf{};

  elf.addString("mstring123");
  BOOST_CHECK_EQUAL(elf.getStringTable().size(), 1);
  BOOST_CHECK_EQUAL(elf.getStringTable().at(0), "mstring123");
  BOOST_CHECK_EQUAL(elf.getSectionHeaders().at(elf.getStringTableIdx()).sh_size,
                    1);
}

BOOST_AUTO_TEST_CASE(elf_test_addSectionName) {
  ELFWrapper elf{};

  elf.addSectionName("msection1");
  BOOST_CHECK_EQUAL(elf.getSectionNames().size(), 3);
  BOOST_CHECK_EQUAL(elf.getSectionNames().at(2), "msection1");
  BOOST_CHECK_EQUAL(
      elf.getSectionHeaders().at(elf.getSectionNamesIdx()).sh_size, 3);
}

BOOST_AUTO_TEST_CASE(elf_test_addSectionHeader) {
  ELFWrapper elf{};

  // Test the copy version
  {
    Elf32_Shdr hdr;

    // This is admittedly lazy
    char* pHdr = reinterpret_cast<char*>(&hdr);
    for (size_t i = 0; i < sizeof(hdr); i++) {
      pHdr[i] = i;
    }
    elf.addSectionHeader(hdr);
    BOOST_CHECK_EQUAL(elf.getSectionHeaders().size(), 3);
    auto& storedHdr = elf.getSectionHeaders().at(2);
    char* pStoredHdr = reinterpret_cast<char*>(&storedHdr);
    for (size_t i = 0; i < sizeof(storedHdr); i++) {
      BOOST_CHECK_EQUAL(pStoredHdr[i], i);
    }
  }

  // Test the rvalue version
  {
    elf.addSectionHeader(Elf32_Shdr{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    BOOST_CHECK_EQUAL(elf.getSectionHeaders().size(), 4);
    auto& storedHdr = elf.getSectionHeaders().at(3);
    BOOST_CHECK_EQUAL(storedHdr.sh_name, 0);
    BOOST_CHECK_EQUAL(storedHdr.sh_type, 1);
    BOOST_CHECK_EQUAL(storedHdr.sh_flags, 2);
    BOOST_CHECK_EQUAL(storedHdr.sh_addr, 3);
    BOOST_CHECK_EQUAL(storedHdr.sh_offset, 4);
    BOOST_CHECK_EQUAL(storedHdr.sh_size, 5);
    BOOST_CHECK_EQUAL(storedHdr.sh_link, 6);
    BOOST_CHECK_EQUAL(storedHdr.sh_info, 7);
    BOOST_CHECK_EQUAL(storedHdr.sh_addralign, 8);
    BOOST_CHECK_EQUAL(storedHdr.sh_entsize, 9);
  }
}

BOOST_AUTO_TEST_CASE(elf_test_setSection) {
  ELFWrapper elf{};

  // Attempting to a string table section should fail
  { BOOST_CHECK_THROW(elf.setSection("strtab"), ELFException); }

  // Changing to a section name that doesn't exist should create a new symbol
  // table and a new relocation table
  {
    auto& hdr = elf.setSection("msection");

    // Check that two new section headers have been added
    BOOST_CHECK_EQUAL(elf.getSectionHeaders().size(), 4);
    BOOST_CHECK_EQUAL(elf.getHeader().e_shnum, 4);

    BOOST_CHECK_EQUAL(hdr.sh_name, 2);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_SYMTAB);
    BOOST_CHECK_EQUAL(hdr.sh_flags, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 1);
    BOOST_CHECK_EQUAL(hdr.sh_link, elf.getStringTableIdx());
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, sizeof(Elf32_Sym));

    // Check that the list of symbol tables actually grew
    BOOST_CHECK_EQUAL(elf.getSymbolTables().size(), 1);

    // Check that the null entry got added at the beginning of the table
    auto& tab = elf.getSymbolTables().at(0);
    BOOST_CHECK_EQUAL(tab.size(), 1);
    auto& nullEnt = tab.at(0);
    char* pNullEnt = reinterpret_cast<char*>(&nullEnt);
    for (size_t i = 0; i < sizeof(nullEnt); i++) {
      BOOST_CHECK_EQUAL(pNullEnt[i], 0);
    }

    // Check that the list of relocation sections actually grew
    BOOST_CHECK_EQUAL(elf.getRelocationSections().size(), 1);

    // Now check the relocation section's header
    auto& relHdr = elf.getSectionHeaders().at(3);
    BOOST_CHECK_EQUAL(relHdr.sh_name, 3);
    BOOST_CHECK_EQUAL(relHdr.sh_type, SHT_REL);
    BOOST_CHECK_EQUAL(relHdr.sh_flags, 0);
    BOOST_CHECK_EQUAL(relHdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(relHdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(relHdr.sh_size, 0);
    // index of msection shdr
    BOOST_CHECK_EQUAL(relHdr.sh_link, 2);
    BOOST_CHECK_EQUAL(relHdr.sh_info, 2);
    BOOST_CHECK_EQUAL(relHdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(relHdr.sh_entsize, sizeof(Elf32_Rel));

    // Check that the names got added to the section names string table
    BOOST_CHECK_EQUAL(elf.getSectionNames().size(), 4);
    BOOST_CHECK_EQUAL(elf.getSectionNames().at(2), "msection");
    BOOST_CHECK_EQUAL(elf.getSectionNames().at(3), "relmsection");

    BOOST_CHECK_EQUAL(elf.getCurrentSectionIdx(), 2);
  }

  // Changing to an existing section should not create any new sections
  {
    elf.setSection("msection");
    BOOST_CHECK_EQUAL(elf.getCurrentSectionIdx(), 2);
    BOOST_CHECK_EQUAL(elf.getSymbolTables().size(), 1);
    BOOST_CHECK_EQUAL(elf.getSectionHeaders().size(), 4);
    BOOST_CHECK_EQUAL(elf.getHeader().e_shnum, 4);
    BOOST_CHECK_EQUAL(elf.getRelocationSections().size(), 1);
    BOOST_CHECK_EQUAL(elf.getSectionNames().size(), 4);
  }

  // TODO what are valid section names? Invalid ones should fail
}

BOOST_AUTO_TEST_CASE(elf_test_addSymbol) {
  ELFWrapper elf{};

  // Attempting to define a symbol when the current section is not a symbol
  // table should throw an exception
  {
    // elf.addSymbol("asdf", 0, 0, 0, 0, 1, false);
    BOOST_CHECK_THROW(elf.addSymbol("asdf", 0, 0, 0, 0, 1, false),
                      ELFException);
  }

  // TODO invalid names should fail
  // TODO invalid sizes should fail
  // TODO invalid types should fail
  // TODO invalid other should fail

  // Prepare to really define some symbols now
  elf.setSection("msection");

  // Attempting to redefine a symbol should throw an exception
  {
    auto& ret =
        elf.addSymbol("asdf", 1234, 0, ELF32_ST_INFO(STT_OBJECT, STB_LOCAL),
                      STV_DEFAULT, 1, false);
    BOOST_CHECK_THROW(elf.addSymbol("asdf", 0, 0, 0, 0, 1, false),
                      ELFException);

    // Make sure the return value is sane
    BOOST_CHECK_EQUAL(ret.st_name, 0);
    BOOST_CHECK_EQUAL(ret.st_value, 1234);
    BOOST_CHECK_EQUAL(ret.st_size, 0);
    BOOST_CHECK_EQUAL(ret.st_info, ELF32_ST_INFO(STT_OBJECT, STB_LOCAL));
    BOOST_CHECK_EQUAL(ret.st_other, STV_DEFAULT);
    BOOST_CHECK_EQUAL(ret.st_shndx, 1);
    BOOST_CHECK_EQUAL(elf.getStringTable().at(ret.st_name), "asdf");

    // Now let's check that our symbol got added properly
    auto& ent = elf.getSymbolTables().at(0).at(1);
    BOOST_CHECK_EQUAL(ent.st_name, 0);
    BOOST_CHECK_EQUAL(ent.st_value, 1234);
    BOOST_CHECK_EQUAL(ent.st_size, 0);
    BOOST_CHECK_EQUAL(ent.st_info, ELF32_ST_INFO(STT_OBJECT, STB_LOCAL));
    BOOST_CHECK_EQUAL(ent.st_other, STV_DEFAULT);
    BOOST_CHECK_EQUAL(ent.st_shndx, 1);
    BOOST_CHECK_EQUAL(elf.getStringTable().at(ent.st_name), "asdf");
  }
}

BOOST_AUTO_TEST_SUITE_END();
