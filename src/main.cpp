

#include "tokenizer.h"

int main(int argc, char *argv[]) {
  auto tokenizer = Tokenizer{};
  auto input = "add a, 32\n";
  std::stringstream stream;
  stream << input;
  auto tokens = tokenizer.tokenize(stream);
  std::cout << tokens->size() << std::endl;
  std::cout << tokens->at(0)  << std::endl;
  std::cout << tokens->at(1)  << std::endl;
  std::cout << tokens->at(2)  << std::endl;
  std::cout << tokens->at(3)  << std::endl;

  return 0;
}
