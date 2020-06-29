

#include <fstream>
#include <getopt.h>

#include "tokenizer.hpp"
#include "elf_writer.hpp"
#include "assembler.hpp"
#include "parser.hpp"

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

  const struct option long_options[] = {
      {"tokenize", no_argument, nullptr, 0},
      {"parse", no_argument, nullptr, 0},
      {nullptr, 0, nullptr, 0},
  };

  bool tokenize_only = false;
  bool parse_only = false;

  int c = 0;
  int option_index = 0;
  while ((c = getopt_long(argc, argv, "tp", long_options, &option_index)) != -1) {
    switch (c) {
      case 0: {
        using namespace std::literals::string_view_literals;
        const char* option_name = long_options[option_index].name;
        if ("tokenize"sv == option_name) {
          tokenize_only = true;
        } else if ("parse"sv == option_name) {
          parse_only = true;
        }
      }
      break;

      case 't':
      tokenize_only = true;
      break;

      case 'p':
      parse_only = true;
      break;

      case '?':
      break;

      default:
      break;
    }

    if (tokenize_only && parse_only) {
      std::cerr << "Only one of --tokenize and --parse may be specified" << std::endl;
      return 1;
    }
  }

  InputFile infile{std::string{argv[optind]}};
  if (!infile.exists()) {
    std::cerr << "Input file does not exist" << std::endl;
    return -1;
  }

  // infile.stream()

  Tokenizer tokenizer{};
  auto token_list = tokenizer.tokenize(infile.stream());
  if (tokenize_only) {
    for (auto& token : token_list) {
      std::cout << token << std::endl;
    }
    return 0;
  }
  Parser parser{token_list};
  auto root_node = parser.parse();
  if (parse_only) {
    // TODO print tree
    return 0;
  }
  Assembler assembler{};
  ELF elf{};
  assembler.assemble(root_node, elf);

  ELFWriter writer{elf};
  writer.write("a.out");


  // ELFReader::read(infile.stream()).or_else([](std::string msg) {
  //   std::cerr << msg << std::endl;
  // });

  return 0;
}
