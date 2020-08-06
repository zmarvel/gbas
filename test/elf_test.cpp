
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
  Elf32_Ehdr& elfHeader = elf.get_header();

  BOOST_CHECK_EQUAL(elfHeader.e_ident[0], ELFMAG0);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[1], ELFMAG1);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[2], ELFMAG2);
  BOOST_CHECK_EQUAL(elfHeader.e_ident[3], ELFMAG3);
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
  // TODO relocation sections
  // BOOST_CHECK_EQUAL(elf.get_sections().size(), 13);
  BOOST_CHECK_EQUAL(elf.get_sections().size(), 8);
  
  // Test the section name string table .shstrtab
  {
    StrTabSection& shStrTab = elf.get_shstring_table();

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
    StrTabSection& strTab = elf.get_string_table();

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
      dynamic_cast<ProgramSection&>(elf.get_section("data"));
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
      dynamic_cast<ProgramSection&>(elf.get_section("rodata"));
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
      dynamic_cast<ProgramSection&>(elf.get_section("bss"));
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
      dynamic_cast<ProgramSection&>(elf.get_section("text"));
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
      dynamic_cast<ProgramSection&>(elf.get_section("init"));
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
      dynamic_cast<SymTabSection&>(elf.get_symbol_table());
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
 * Make sure add_section behavior is correct.
 */
BOOST_AUTO_TEST_CASE(elf_test_add_section) {
  // TODO invalid section name should fail

  // adding duplicate name should fail
  {
    ELFWrapper elf{};
    BOOST_CHECK_THROW(elf.add_section(std::make_unique<SymTabSection>("symtab",
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
    size_t prevCount = elf.get_sections().size();
    elf.add_section(std::make_unique<SymTabSection>("new_section",
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
    BOOST_CHECK_EQUAL(elf.get_sections().size(), prevCount + 1);
    BOOST_CHECK(elf.get_section("new_section").header().sh_type == SHT_PROGBITS);
  }

  // with relocation section should mean two sections got added, one of type
  // SHT_REL
  {
    ELFWrapper elf{};
    size_t prevCount = elf.get_sections().size();
    elf.add_section(std::make_unique<SymTabSection>("new_section",
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
    BOOST_CHECK_EQUAL(elf.get_sections().size(), prevCount + 2);
    BOOST_CHECK(elf.get_section("new_section").header().sh_type == SHT_PROGBITS);
    BOOST_CHECK(elf.get_section("relnew_section").header().sh_type == SHT_REL);
  }
}

/**
 * Make sure we can add a string to the string table and pull it back out.
 */
BOOST_AUTO_TEST_CASE(elf_test_add_string) {
  ELFWrapper elf{};

  elf.add_string("mstring123");
  // The first entry is null/empty--should we report it in the string table size?
  BOOST_CHECK_EQUAL(elf.get_string_table().strings().size(), 2);
  BOOST_CHECK_EQUAL(elf.get_string_table().strings().at(1), "mstring123");
  BOOST_CHECK_EQUAL(elf.get_string_table().header().sh_size,
                    11);
}

/**
 * Make sure we can define a symbol in a section that that it gets added
 * correctly to the symbol table.
 */
BOOST_AUTO_TEST_CASE(elf_test_add_symbol) {
  // TODO invalid names should fail
  // TODO invalid sizes should fail
  // TODO invalid types should fail
  // TODO invalid other should fail

  // Attempting to redefine a symbol should throw an exception
  {
    ELFWrapper elf{};
    elf.set_section("data");

    elf.add_symbol("asdf", 1234, 0,
        ISection::Type{}.object(), ISection::Binding{}.local(),
        ISection::Visibility{}, false);
    BOOST_CHECK_THROW(elf.add_symbol("asdf", 0, 0, ISection::Type{}.object(),
          ISection::Binding{}.local(), ISection::Visibility{}, false),
        ELFException);
  }

  // Should be able to define a symbol once and pull it back out
  {
    ELFWrapper elf{};
    elf.set_section("data");

    auto& ret = elf.add_symbol("asdf", 1234, 0,
        ISection::Type{}.object(), ISection::Binding{}.local(),
        ISection::Visibility{}, false);

    // Make sure the return value is sane
    // 1 because the first entry in every string table is null/empty
    BOOST_CHECK_EQUAL(ret.st_name, 1);
    BOOST_CHECK_EQUAL(ret.st_value, 1234);
    BOOST_CHECK_EQUAL(ret.st_size, 0);
    BOOST_CHECK_EQUAL(ret.st_info, ELF32_ST_INFO(STB_LOCAL, STT_OBJECT));
    BOOST_CHECK_EQUAL(ret.st_other, STV_DEFAULT);
    BOOST_CHECK_EQUAL(ret.st_shndx, elf.get_section_idx("data"));
    BOOST_CHECK_EQUAL(elf.get_string_table().strings().at(ret.st_name), "asdf");

    // Now let's check that our symbol got added properly
    auto& ent = elf.get_symbol_table().symbols().at(1);
    // 1 because the first entry in every string table is null/empty
    BOOST_CHECK_EQUAL(ent.st_name, 1);
    BOOST_CHECK_EQUAL(ent.st_value, 1234);
    BOOST_CHECK_EQUAL(ent.st_size, 0);
    BOOST_CHECK_EQUAL(ent.st_info, ELF32_ST_INFO(STB_LOCAL, STT_OBJECT));
    BOOST_CHECK_EQUAL(ent.st_other, STV_DEFAULT);
    BOOST_CHECK_EQUAL(ent.st_shndx, elf.get_section_idx("data"));
    BOOST_CHECK_EQUAL(elf.get_string_table().strings().at(ent.st_name), "asdf");
  }

  // Should be able to define a relocatable symbol and find correct relocation
  // information in the expected relocation section
  if (false) {
    ELFWrapper elf{};
    elf.set_section("data");

    auto& ret = elf.add_symbol("asdf", 5678, 0,
        ISection::Type{}.function(), ISection::Binding{}.global(),
        ISection::Visibility{}, true);

    auto& relTab = dynamic_cast<RelSection&>(elf.get_section("reldata"));
    BOOST_CHECK_EQUAL(relTab.relocations().size(), 1);
    auto& relEnt = relTab.relocations().at(0);
    BOOST_CHECK_EQUAL(relEnt.r_offset, 5678);
    BOOST_CHECK_EQUAL(ELF32_R_SYM(relEnt.r_info),
        elf.get_symbol_table().symbols().size() - 1);
    BOOST_CHECK_EQUAL(ELF32_R_TYPE(relEnt.r_info), R_386_32);
  }
}

/**
 * Add some data to a section and make sure it can be read back out.
 */
BOOST_AUTO_TEST_CASE(elf_test_addData) {
  ELFWrapper elf{};

  std::vector<uint8_t> data(256);
#if 0
  for (size_t i = 0; i < data.size(); i++) {
    data[i] = static_cast<uint8_t>(i * 2);
  }
#endif
  if (false) {
    auto& rData = dynamic_cast<ProgramSection&>(elf.get_section("data"));
    rData.append(data);

    BOOST_CHECK_EQUAL(rData.data().size(), data.size());
    BOOST_CHECK_EQUAL(rData.header().sh_size, 256);
    for (size_t i = 0; i < data.size(); i++) {
      BOOST_CHECK_EQUAL(data[i], rData.data()[i]);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END();
