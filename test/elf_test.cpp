
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <numeric>

#include "elf.hpp"
#include "elf_wrapper.hpp"

using namespace GBAS;

BOOST_AUTO_TEST_SUITE(elf);

/**
 * Make sure the header member of the elf file is filled out as far as it can
 * be.
 */
BOOST_AUTO_TEST_CASE(elf_test_constructor_header) {

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
  // This is 0 until just before we write out the file. Section objects own
  // their headers, and ELF.mSections can be traversed to figure out the number
  // of section headers.
  BOOST_CHECK_EQUAL(elfHeader.e_shnum, 0);
}

/**
 * Check that all the "default" sections have been created and that their
 * headers are sane.
 */
BOOST_AUTO_TEST_CASE(elf_test_constructor_sections) {
  ELFWrapper elf{};

  // shstrtab, strtab, data, reldata, rodata, relrodata, bss, relbss, text,
  // reltext, init, relinit, symtab
  BOOST_CHECK_EQUAL(elf.getSections().size(), 13);
  
  // Test the section name string table .shstrtab
  {
    StrTabSection& shStrTab = elf.getShStringTable();

    BOOST_CHECK_EQUAL(shStrTab.name(), "shstrtab");
    Elf32_Shdr& hdr = shStrTab.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_STRTAB);
    BOOST_CHECK_EQUAL(hdr.sh_flags, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the string table .strtab
  {
    StrTabSection& strTab = elf.getStringTable();

    BOOST_CHECK_EQUAL(strTab.name(), "strtab");
    Elf32_Shdr& hdr = strTab.header();
    // sh_name is filled out just before writing the structure to a real ELF
    // file.
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
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

  // Test the data section .data
  {
    ProgramSection& progbits =
      dynamic_cast<ProgramSection&>(elf.getSection("data"));
    BOOST_CHECK_EQUAL(progbits.name(), "data");
    Elf32_Shdr& hdr = progbits.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_PROGBITS);
    BOOST_CHECK_EQUAL(hdr.sh_flags, SHF_ALLOC | SHF_WRITE);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the read-only data section .rodata
  {
    ProgramSection& progbits =
      dynamic_cast<ProgramSection&>(elf.getSection("rodata"));
    BOOST_CHECK_EQUAL(progbits.name(), "rodata");
    Elf32_Shdr& hdr = progbits.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_PROGBITS);
    BOOST_CHECK_EQUAL(hdr.sh_flags, SHF_ALLOC);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the uninitialized data section .bss
  {
    ProgramSection& progbits =
      dynamic_cast<ProgramSection&>(elf.getSection("bss"));
    BOOST_CHECK_EQUAL(progbits.name(), "bss");
    Elf32_Shdr& hdr = progbits.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_NOBITS);
    BOOST_CHECK_EQUAL(hdr.sh_flags, SHF_ALLOC | SHF_WRITE);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the executable code section .text
  {
    ProgramSection& progbits =
      dynamic_cast<ProgramSection&>(elf.getSection("text"));
    BOOST_CHECK_EQUAL(progbits.name(), "text");
    Elf32_Shdr& hdr = progbits.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_PROGBITS);
    BOOST_CHECK_EQUAL(hdr.sh_flags, SHF_ALLOC | SHF_EXECINSTR);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the executable initialization code section .init
  {
    ProgramSection& progbits =
      dynamic_cast<ProgramSection&>(elf.getSection("init"));
    BOOST_CHECK_EQUAL(progbits.name(), "init");
    Elf32_Shdr& hdr = progbits.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_PROGBITS);
    BOOST_CHECK_EQUAL(hdr.sh_flags, SHF_ALLOC | SHF_EXECINSTR);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, 0);
  }

  // Test the symbol table
  {
    SymTabSection& symtab = 
      dynamic_cast<SymTabSection&>(elf.getSymbolTable());
    BOOST_CHECK_EQUAL(symtab.name(), "symtab");
    Elf32_Shdr& hdr = symtab.header();
    BOOST_CHECK_EQUAL(hdr.sh_name, 0);
    BOOST_CHECK_EQUAL(hdr.sh_type, SHT_SYMTAB);
    BOOST_CHECK_EQUAL(hdr.sh_flags, SHF_ALLOC);
    BOOST_CHECK_EQUAL(hdr.sh_addr, 0);
    BOOST_CHECK_EQUAL(hdr.sh_offset, 0);
    BOOST_CHECK_EQUAL(hdr.sh_size, 0);
    BOOST_CHECK_EQUAL(hdr.sh_link, 0);
    BOOST_CHECK_EQUAL(hdr.sh_info, 0);
    BOOST_CHECK_EQUAL(hdr.sh_addralign, 0);
    BOOST_CHECK_EQUAL(hdr.sh_entsize, sizeof(Elf32_Sym));
  }

}

/**
 * Make sure addSection behavior is correct.
 */
BOOST_AUTO_TEST_CASE(elf_test_addSection) {
  // TODO invalid section name should fail

  // adding duplicate name should fail
  {
    ELFWrapper elf{};
    BOOST_CHECK_THROW(elf.addSection(std::make_unique<SymTabSection>("symtab",
            Elf32_Shdr{.sh_name = 0,
            .sh_type = SHT_SYMTAB,
            .sh_flags = SHF_ALLOC,
            .sh_addr = 0,
            .sh_offset = 0,
            .sh_size = 0,
            .sh_link = 0,
            .sh_info = 0,
            .sh_addralign = 0,
            .sh_entsize = sizeof(Elf32_Sym)}), false), ELFException);
  }

  // without relocation section should mean only one section got added
  {
    ELFWrapper elf{};
    size_t prevCount = elf.getSections().size();
    elf.addSection(std::make_unique<SymTabSection>("new_section",
          Elf32_Shdr{.sh_name = 0,
          .sh_type = SHT_PROGBITS,
          .sh_flags = SHF_ALLOC,
          .sh_addr = 0,
          .sh_offset = 0,
          .sh_size = 0,
          .sh_link = 0,
          .sh_info = 0,
          .sh_addralign = 0,
          .sh_entsize = sizeof(Elf32_Sym)}), false);
    BOOST_CHECK_EQUAL(elf.getSections().size(), prevCount + 1);
    BOOST_CHECK(elf.getSection("new_section").header().sh_type == SHT_PROGBITS);
  }

  // with relocation section should mean two sections got added, one of type
  // SHT_REL
  {
    ELFWrapper elf{};
    size_t prevCount = elf.getSections().size();
    elf.addSection(std::make_unique<SymTabSection>("new_section",
          Elf32_Shdr{.sh_name = 0,
          .sh_type = SHT_PROGBITS,
          .sh_flags = SHF_ALLOC,
          .sh_addr = 0,
          .sh_offset = 0,
          .sh_size = 0,
          .sh_link = 0,
          .sh_info = 0,
          .sh_addralign = 0,
          .sh_entsize = sizeof(Elf32_Sym)}), true);
    BOOST_CHECK_EQUAL(elf.getSections().size(), prevCount + 2);
    BOOST_CHECK(elf.getSection("new_section").header().sh_type == SHT_PROGBITS);
    BOOST_CHECK(elf.getSection("relnew_section").header().sh_type == SHT_REL);
  }
}

/**
 * Make sure we can add a string to the string table and pull it back out.
 */
BOOST_AUTO_TEST_CASE(elf_test_addString) {
  ELFWrapper elf{};

  elf.addString("mstring123");
  BOOST_CHECK_EQUAL(elf.getStringTable().strings().size(), 1);
  BOOST_CHECK_EQUAL(elf.getStringTable().strings().at(0), "mstring123");
  BOOST_CHECK_EQUAL(elf.getStringTable().header().sh_size,
                    11);
}

/**
 * Make sure we can define a symbol in a section that that it gets added
 * correctly to the symbol table.
 */
BOOST_AUTO_TEST_CASE(elf_test_addSymbol) {
  // TODO invalid names should fail
  // TODO invalid sizes should fail
  // TODO invalid types should fail
  // TODO invalid other should fail

  // Attempting to redefine a symbol should throw an exception
  {
    ELFWrapper elf{};
    elf.setSection("data");

    elf.addSymbol("asdf", 1234, 0,
        ISection::Type{}.object(), ISection::Binding{}.local(),
        ISection::Visibility{}, false);
    BOOST_CHECK_THROW(elf.addSymbol("asdf", 0, 0, ISection::Type{}.object(),
          ISection::Binding{}.local(), ISection::Visibility{}, false),
        ELFException);
  }

  // Should be able to define a symbol once and pull it back out
  {
    ELFWrapper elf{};
    elf.setSection("data");

    auto& ret = elf.addSymbol("asdf", 1234, 0,
        ISection::Type{}.object(), ISection::Binding{}.local(),
        ISection::Visibility{}, false);

    // Make sure the return value is sane
    BOOST_CHECK_EQUAL(ret.st_name, 0);
    BOOST_CHECK_EQUAL(ret.st_value, 1234);
    BOOST_CHECK_EQUAL(ret.st_size, 0);
    BOOST_CHECK_EQUAL(ret.st_info, ELF32_ST_INFO(STB_LOCAL, STT_OBJECT));
    BOOST_CHECK_EQUAL(ret.st_other, STV_DEFAULT);
    BOOST_CHECK_EQUAL(ret.st_shndx, elf.getSectionIdx("data"));
    BOOST_CHECK_EQUAL(elf.getStringTable().strings().at(ret.st_name), "asdf");

    // Now let's check that our symbol got added properly
    auto& ent = elf.getSymbolTable().symbols().at(1);
    BOOST_CHECK_EQUAL(ent.st_name, 0);
    BOOST_CHECK_EQUAL(ent.st_value, 1234);
    BOOST_CHECK_EQUAL(ent.st_size, 0);
    BOOST_CHECK_EQUAL(ent.st_info, ELF32_ST_INFO(STB_LOCAL, STT_OBJECT));
    BOOST_CHECK_EQUAL(ent.st_other, STV_DEFAULT);
    BOOST_CHECK_EQUAL(ent.st_shndx, elf.getSectionIdx("data"));
    BOOST_CHECK_EQUAL(elf.getStringTable().strings().at(ent.st_name), "asdf");
  }

  // Should be able to define a relocatable symbol and find correct relocation
  // information in the expected relocation section
  {
    ELFWrapper elf{};
    elf.setSection("data");

    auto& ret = elf.addSymbol("asdf", 5678, 0,
        ISection::Type{}.function(), ISection::Binding{}.global(),
        ISection::Visibility{}, true);

    auto& relTab = dynamic_cast<RelSection&>(elf.getSection("reldata"));
    BOOST_CHECK_EQUAL(relTab.relocations().size(), 1);
    auto& relEnt = relTab.relocations().at(0);
    BOOST_CHECK_EQUAL(relEnt.r_offset, 5678);
    BOOST_CHECK_EQUAL(ELF32_R_SYM(relEnt.r_info),
        elf.getSymbolTable().symbols().size() - 1);
    BOOST_CHECK_EQUAL(ELF32_R_TYPE(relEnt.r_info), R_386_32);
  }
}

/**
 * Add some data to a section and make sure it can be read back out.
 */
BOOST_AUTO_TEST_CASE(elf_test_addData) {
  ELFWrapper elf{};

  std::vector<uint8_t> data(256);
  for (size_t i = 0; i < data.size(); i++) {
    data[i] = static_cast<uint8_t>(i * 2);
  }
  {
    auto& rData = dynamic_cast<ProgramSection&>(elf.getSection("data"));
    rData.append(data);

    BOOST_CHECK_EQUAL(rData.data().size(), data.size());
    BOOST_CHECK_EQUAL(rData.header().sh_size, 256);
    for (size_t i = 0; i < data.size(); i++) {
      BOOST_CHECK_EQUAL(data[i], rData.data()[i]);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END();
