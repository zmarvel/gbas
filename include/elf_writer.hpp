
#ifndef ELF_WRITER_HPP
#define ELF_WRITER_HPP

#include <iostream>

#include "elf.hpp"

namespace GBAS {

class ELFWriter {
  public:
    ELFWriter(ELF& elf) : elf_{elf} { }
    ELFWriter() = delete;

    void write(std::ostream& os);
    size_t populate_section_headers(size_t starting_offs);

  private:
    ELF& elf_;
};

}

#endif // ELF_WRITER_HPP
