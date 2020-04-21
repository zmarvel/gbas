
#include "elf_reader.hpp"

using namespace GBAS;

bool check_ident(unsigned char ident[EI_NIDENT]) {
    const char expected[] = {
        0x7f, 'E', 'L', 'F',
    };

    return strncmp(reinterpret_cast<char*>(ident), expected, sizeof(expected)) == 0;
}

tl::expected<std::unique_ptr<ELFReader>, std::string>
ELFReader::read(std::istream& is)
{
  auto pelf = std::make_unique<ELF>();

  auto& header = pelf->header();

  {
    auto buf = std::make_unique<char[]>(sizeof(header));
    if (is.read(buf.get(), sizeof(header))) {
      memcpy(&header, buf.get(), sizeof(header));
    } else {
      return tl::make_unexpected("Failed to read header from input stream");
    }
  }

  if (!check_ident(header.e_ident)) {
    std::stringstream ss;
    ss << "Invalid file header: ";
    ss << header.e_ident;
    return tl::make_unexpected(ss.str());
  }

  if (header.e_ident[4] != ELFCLASS32) {
    return tl::make_unexpected("Only 32-bit ELF is supported");
  }

  off_t shoff = header.e_shoff;
  // This assumes all the content we care about in the file follows the section
  // header table
  std::cout << "shoff: " << header.e_shoff << std::endl;

  if (!is.seekg(shoff, std::ios_base::beg)) {
    return tl::make_unexpected("Unable to seek to section header table");
  }

  std::cout << "shentsize: " << header.e_shentsize << std::endl;
  std::cout << "shnum: " << header.e_shnum << std::endl;
  std::vector<Elf32_Shdr> shtab{};
  {
    size_t shsize = header.e_shentsize * header.e_shnum;
    std::cout << "section header table size: " << shsize << std::endl;
    auto pshbuf = std::make_unique<char[]>(shsize);
    if (is.read(pshbuf.get(), shsize)) {
      for (size_t i = 0; i < header.e_shnum; i++) {
        Elf32_Shdr shdr;
        memcpy(&shdr, pshbuf.get() + i*sizeof(shdr), sizeof(shdr));
        shtab.push_back(shdr);
      }
    } else {
      return tl::make_unexpected("Unable to read section header table");
    }
  }

  for (auto it = shtab.begin(); it != shtab.end(); it++) {
    std::cout
      << "name: " << it->sh_name << std::endl
      << "type: " << it->sh_type << std::endl
      << "flags: " << it->sh_flags << std::endl
      << "addr: " << it->sh_addr << std::endl
      << "offset: " << it->sh_offset << std::endl
      << "size: " << it->sh_size << std::endl
      << "link: " << it->sh_link << std::endl
      << "info: " << it->sh_info << std::endl
      << "addralign: " << it->sh_addralign << std::endl
      << "entsize: " << it->sh_entsize << std::endl << std::endl;
  }

  auto &strhdr = shtab[header.e_shstrndx];

  if (!is.seekg(strhdr.sh_offset, std::ios_base::beg)) {
    return tl::make_unexpected("Unable to seek to section name string table");
  }

  std::vector<std::string> section_names{};
  {
    {
      char buf;
      if (!is.read(&buf, sizeof(buf)) || buf != '\0') {
        return tl::make_unexpected("Invalid section name string table");
      }
    }
    std::string curr{};
    for (size_t i = 0; i < strhdr.sh_size; i++) {
      char buf;
      if (!is.read(&buf, sizeof(buf))) {
        return tl::make_unexpected(
            "Failed to read char from section name string table");
      }
      curr.push_back(buf);
      if (buf == '\0') {
        std::cout << curr << std::endl;
        section_names.push_back(curr);
        curr.clear();
      }
    }
  }

  auto reader = std::make_unique<ELFReader>();
  reader->elf_ = std::move(*pelf);
  return std::move(reader);
}
