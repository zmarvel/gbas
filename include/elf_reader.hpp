
#ifndef ELF_READER_HPP
#define ELF_READER_HPP

#include <iostream>
#include <memory>

#include "tl/expected.hpp"

#include "elf.hpp"

namespace GBAS {

class ELFReader {
  public:
    ELFReader() : elf_{} { }

    static tl::expected<std::unique_ptr<ELFReader>, std::string> read(
      std::istream& is);

  private:
    ELF elf_;
};

}

#endif // ELF_READER_HPP
