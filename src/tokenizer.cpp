
#include <algorithm>
#include <string.h>

#include "tokenizer.h"

const std::array<Token, 2> Tokenizer::reserved = {
  "EOL",
  "EOF",
};

TokenList::TokenList() {
}

void TokenList::add(Token tok) {
  if (Tokenizer::isReserved(tok)) {
    throw TokenizerException(tok + " is reserved");
  }
  push_back(tok);
}

void TokenList::add_reserved(Token tok) {
  push_back(tok);
}


bool Tokenizer::isReserved(Token tok) {
  auto it = std::find(reserved.begin(), reserved.end(), tok);
  return it != reserved.end();
}


TokenList *Tokenizer::tokenize(std::basic_istream<char> &lines) {
  auto tokens = new TokenList();

  int lineno = 0;
  for (std::array<char, MAX_LINE_LEN> line; lines.getline(&line[0], sizeof(line));) {
    size_t begin = 0;
    for (size_t pos = 0; pos < MAX_LINE_LEN; pos++) {
      char curr = line[pos];
      if ((curr == '\n') || (curr == '\0') || (curr == ';')) {
        if (begin < pos) {
          auto tok = Token{&line[begin], pos-begin};
          tokens->add(tok);
        }
        tokens->add_reserved("EOL");
        lineno++;
        break;
      } else if ((curr == ' ') || (curr == ',')) {
        if (begin < pos) {
          auto tok = Token{&line[begin], pos-begin};
          tokens->add(tok);
        }
        begin = pos + 1;
      }
    }
  }

  tokens->add_reserved("EOF");

  return tokens;
}
