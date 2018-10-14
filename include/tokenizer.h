
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>



typedef std::string Token;


class TokenizerException : std::exception {
  public:
    TokenizerException(const char *msg) {
      this->msg = msg;
    }

    TokenizerException(const std::string &msg) {
      this->msg = msg;
    }

    virtual const char* what() const throw() {
      return msg.c_str();
    };

  private:
    std::string msg;
};



class TokenList : public std::vector<Token> {
  public:
    TokenList();
    ~TokenList();

    using vector::at;

    void add(Token tok);
    void add_reserved(Token tok);
};



const int MAX_LINE_LEN = 128;
class Tokenizer {
  public:
    TokenList *tokenize(std::basic_istream<char> &lines);
    static bool isReserved(Token tok);
    static bool isOperator(char c);
    static bool isAlphaNumeric(char c);

  private:
    static const std::array<Token, 2> reserved;
    static const std::array<char, 4> operators;
    void logError(std::ostream &out, const std::string &msg,
                  const std::string &line, int lineno, int col);

    enum class State {
      START_LINE,
      START_TOKEN,
      STRING_TOKEN,
      TOKEN,
      END_TOKEN,
      END_LINE,
    };
};
