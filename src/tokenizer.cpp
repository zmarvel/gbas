
#include <algorithm>
#include <string>

#include "tokenizer.hpp"
#include "char_utils.hpp"

using namespace GBAS;

const std::array<Token, 2> Tokenizer::reserved = {
    "EOL",
    "EOF",
};

bool Tokenizer::isReserved(Token tok) {
  auto it = std::find(reserved.begin(), reserved.end(), tok);
  return it != reserved.end();
}

void Tokenizer::logError(std::ostream& out, const std::string& msg,
                         const std::string& line, int lineno, int col) {
  out << "Line " << lineno << ": " << msg << std::endl;
  out << line << std::endl;
  std::string arrow(col + 1, '-');
  arrow[col] = '^';
  out << arrow << std::endl;
}

TokenList Tokenizer::tokenize(std::basic_istream<char>& lines) {
  TokenList tokens = TokenList{};

  int lineno = 0;
  std::string tokbuf;
  for (std::string line; std::getline(lines, line); line.clear()) {
    auto state = State::START_LINE;

    size_t pos = 0;
    while (state != State::END_LINE) {
      char curr = line[pos];
      // std::cout << curr << std::endl;
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
          if (pos >= line.size() || curr == ';') {
            // EOL or comment
            lineno++;
            state = State::END_LINE;
          } else if (curr == ' ') {
            // skip spaces but keep track of position for error messages
            pos++;
          } else if (isAlphaNumeric(curr) || curr == '.' || curr == '_' ||
                     curr == '\\') {
            // start of regular token
            state = State::TOKEN;
          } else if (curr == '"') {
            // opening quote for string---requires special handling so spaces
            // don't indicate the end of a token
            tokbuf.push_back(curr);
            pos++;
            state = State::STRING_TOKEN;
          } else if (curr == '(' || curr == ')' || curr == ',') {
            // one-symbol tokens
            tokbuf.push_back(curr);
            pos++;
            state = State::END_TOKEN;
          } else if (isNumericOp(curr)) {
            tokbuf.push_back(curr);
            pos++;
            state = State::END_TOKEN;
          } else {
            logError(std::cerr, "Invalid token", std::string{line.c_str()},
                     lineno, pos);
            throw TokenizerException("Invalid token" + line, lineno, pos);
          }
          break;
        case State::STRING_TOKEN:
          if (pos >= line.size()) {
            logError(std::cerr, "Unterminated string", std::string{line.c_str()},
                     lineno, pos);
            throw TokenizerException("Unterminated string", lineno, pos);
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
          } else if (isNumericOp(curr)) {
            state = State::END_TOKEN;
          } else if (curr == ',' || curr == ')') {
            // end of token, but don't increment pos so these symbols are
            // stored themselves as a token
            state = State::END_TOKEN;
          } else if (isAlphaNumeric(curr) || curr == '.' || curr == '_' ||
                     curr == '\\') {
            tokbuf.push_back(curr);
            pos++;
          } else {
            logError(std::cerr, "Invalid token", std::string{line.data()},
                     lineno, pos);
            throw TokenizerException("Invalid token " + line, lineno, pos);
          }
          break;
        case State::END_TOKEN:
          tokens.push_back(Token{tokbuf});
          tokbuf.clear();
          state = State::START_TOKEN;
          break;
        case State::END_LINE:
          break;
      }
    }
    tokens.push_back("EOL");
  }
  tokens.push_back("EOF");

  return tokens;
}
