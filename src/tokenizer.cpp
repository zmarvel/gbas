
#include <algorithm>
#include <string>

#include "tokenizer.h"

const std::array<Token, 2> Tokenizer::reserved = {
  "EOL",
  "EOF",
};

const std::array<char, 4> Tokenizer::operators = {
  '*',
  '/',
  '+',
  '-'
};



TokenList::TokenList() : std::vector<Token>() {
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

bool Tokenizer::isAlphaNumeric(char c) {
  return ((c >= '0') && (c <= '9')) ||
    ((c >= 'A') && (c <= 'Z')) ||
    ((c >= 'a') && (c <= 'z'));
}

bool Tokenizer::isOperator(char c) {
  auto it = std::find(operators.begin(), operators.end(), c);
  return it != operators.end();
}

void Tokenizer::logError(std::ostream &out, const std::string &msg,
                         const std::string &line, int lineno, int col) {
  out << "Line " << lineno << ": " << msg << std::endl;
  out << line << std::endl;
  std::string arrow(col+1, '-');
  arrow[col] = '^';
  out << arrow << std::endl;
}

TokenList *Tokenizer::tokenize(std::basic_istream<char> &lines) {
  auto tokens = new TokenList();

  int lineno = 0;
  std::string tokbuf;
  for (std::string line; std::getline(lines, line); line.clear()) {
    auto state = State::START_LINE;

    size_t pos = 0;
    while (state != State::END_LINE) {
      char curr = line[pos];
      //std::cout << curr << std::endl;
      switch (state) {
        case State::START_LINE:
          if (pos >= line.size()) {
            // EOL
            lineno++;
            state = State::END_LINE;
          } else if (curr == ' ') {
            pos++;
          } else {
            state = State::START_TOKEN;
          }
          break;
        case State::START_TOKEN:
          tokbuf.clear();
          if (pos >= line.size() ||
              curr == ';') {
            // EOL or comment
            lineno++;
            state = State::END_LINE;
          } else if (curr == ' ') {
            // skip spaces but keep track of position for error messages
            pos++;
          } else if (isAlphaNumeric(curr) ||
                     curr == '.' ||
                     curr == '_' ||
                     curr == '\\') {
            // start of regular token
            state = State::TOKEN;
          } else if (curr == '"') {
            // opening quote for string---requires special handling so spaces
            // don't indicate the end of a token
            tokbuf.push_back(curr);
            pos++;
            state = State::STRING_TOKEN;
          } else if (curr == '(' ||
                     curr == ')' ||
                     curr == ',') {
            // one-symbol tokens
            tokbuf.push_back(curr);
            pos++;
            state = State::END_TOKEN;
          } else if (isOperator(curr)) {
            tokbuf.push_back(curr);
            pos++;
            state = State::END_TOKEN;
          } else {
            logError(std::cerr, "Invalid token",
                     std::string{line.data()}, lineno, pos);
            return nullptr;
          }
          break;
        case State::STRING_TOKEN:
          if (pos >= line.size()) {
            logError(std::cerr, "Unterminated string",
                     std::string{line.data()}, lineno, pos);
            return nullptr;
          } else if (curr == '"') {
            // string end
            tokbuf.push_back(curr);
            pos++;
            state = State::END_TOKEN;
          } else {
            // mid-string
            tokbuf.push_back(curr);
            pos++;
          }
          break;
        case State::TOKEN:
          if (pos >= line.size()) {
            // EOL
            state = State::END_TOKEN;
          } else if (curr == ' ') {
            pos++;
            state = State::END_TOKEN;
          } else if (isOperator(curr)) {
            state = State::END_TOKEN;
          } else if (curr == ',' ||
                     curr == ')') {
            // end of token, but don't increment pos so these symbols are
            // stored themselves as a token
            state = State::END_TOKEN;
          } else if (isAlphaNumeric(curr) ||
                     curr == '.' ||
                     curr == '_' ||
                     curr == '\\') {
            tokbuf.push_back(curr);
            pos++;
          } else {
            logError(std::cerr, "Invalid token",
                     std::string{line.data()}, lineno, pos);
            return nullptr;
          }
          break;
        case State::END_TOKEN:
          tokens->add(Token{tokbuf});
          tokbuf.clear();
          state = State::START_TOKEN;
          break;
        case State::END_LINE:
          break;
      }
    }
    tokens->add_reserved("EOL");
  }
  tokens->add_reserved("EOF");

  return tokens;
}
