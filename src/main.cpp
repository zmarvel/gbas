

#include <fstream>
#include "tokenizer.h"

class InputFile {
  public:
    InputFile(std::string filename) :
       filename{filename},
       stream{filename}
    { }

    ~InputFile() {
      stream.close();
    }

    bool exists() {
      return stream.is_open();
    }

    std::ifstream &getStream() {
      return stream;
    }

  private:
    std::string filename;
    std::ifstream stream;
};

static const std::string USAGE = " <input file>";
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << argv[0] << USAGE << std::endl;
    return -1;
  }

  InputFile infile{std::string{argv[1]}};
  if (!infile.exists()) {
    std::cerr << "Input file does not exist" << std::endl;
    return -1;
  }

  auto tokenizer = Tokenizer{};
  // Have to explicitly type because tokens is inferred wrong
  // TODO investigate
  TokenList *tokens = tokenizer.tokenize(infile.getStream());
  std::cout << tokens->size() << std::endl;
  for (auto it = tokens->begin(); it != tokens->end(); it++) {
    std::cout << *it  << std::endl;
  }

  return 0;
}
