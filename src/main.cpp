

#include <fstream>

#include "tokenizer.hpp"
#include "elf_reader.hpp"

class InputFile {
 public:
  InputFile(std::string filename) : filename_{filename}, stream_{filename} {}

  ~InputFile() { stream_.close(); }

  bool exists() { return stream_.is_open(); }

  std::ifstream& stream() { return stream_; }

 private:
  std::string filename_;
  std::ifstream stream_;
};

using namespace GBAS;

static const std::string USAGE = " <input file>";
int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << argv[0] << USAGE << std::endl;
    return -1;
  }

  InputFile infile{std::string{argv[1]}};
  if (!infile.exists()) {
    std::cerr << "Input file does not exist" << std::endl;
    return -1;
  }

  ELFReader::read(infile.stream()).or_else([](std::string msg) {
    std::cerr << msg << std::endl;
  });


  return 0;
}
