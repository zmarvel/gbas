
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using Token = std::string;

class TokenizerException : std::exception {
 public:
  TokenizerException(const char* msg, int line, int col) : mMsg{msg}, mLine{line}, mCol{col} {}

  TokenizerException(const std::string& msg, int line, int col) : mMsg{msg}, mLine{line}, mCol{col} {}

  virtual const char* what() const throw() { return mMsg.c_str(); };

 private:
  std::string mMsg;
  int mLine;
  int mCol;
};

using TokenList = std::vector<Token>;

const int MAX_LINE_LEN = 128;
class Tokenizer {
 public:
  TokenList tokenize(std::basic_istream<char>& lines);
  static bool isReserved(Token tok);
  static bool isOperator(char c);

 private:
  static const std::array<Token, 2> reserved;
  static const std::array<char, 4> operators;
  void logError(std::ostream& out, const std::string& msg,
                const std::string& line, int lineno, int col);

  enum class State {
    START_LINE,
    START_TOKEN,
    STRING_TOKEN,
    TOKEN,
    END_TOKEN,
    END_LINE,
  };
};

#endif  // TOKENIZER_H
