
#include <algorithm>

#include "parser.h"

Parser::Parser() {

}

Parser::~Parser() {

}



static const std::array<const Token, 8> registers = {
  "a",
  "f",
  "b",
  "c",
  "d",
  "e",
  "h",
  "l",
};

static const std::array<const Token, 6> doubleRegisters = {
  "af",
  "bc",
  "de",
  "hl",
  "sp",
  "pc",
};

static const std::array<const Token, 2> instructions = {
  "inc",
  "nop",
};



#if 0
template<class T>
AST::Node<T> Parser::parse(TokenList *tokens, int i) {

  Token currTok = tokens->at(i);
  if (std::find(registers.begin(), registers.end(), currTok) != registers.end()) {
    
  } else if (std::find(doubleRegisters.begin(), doubleRegisters.end(), currTok)
             != doubleRegisters.end()) {
  } else if (std::find(instructions.begin(), instructions.end(), currTok)
             != instructions.end()) {
    if (currTok == "inc") {
      return AST::Node<AST::Instruction1<AST::Node<AST::Register<'a'>>>>{parse<AST::Register<'a'>>(tokens, i+1)};
    }
    //else if (currTok == "nop") {
    //  return AST::Node<AST::Instruction<0>>{};
    //}
  }
}
#endif

template<char R>
AST::Register<R> Parser::parseRegister(Token& t) {
  switch (t.at(0)) {
    case 'a': return AST::Node<AST::Register<'a'>>{};
    case 'f': return AST::Node<AST::Register<'f'>>{};
    case 'b': return AST::Node<AST::Register<'b'>>{};
    case 'c': return AST::Node<AST::Register<'c'>>{};
    case 'd': return AST::Node<AST::Register<'d'>>{};
    case 'e': return AST::Node<AST::Register<'e'>>{};
    case 'h': return AST::Node<AST::Register<'h'>>{};
    case 'l': return AST::Node<AST::Register<'l'>>{};
  }
}

template<char R1, char R2>
AST::DRegister<R1, R2> Parser::parseDRegister(Token& t) {
  if (t == "af") {
    return AST::DRegister<'a', 'f'>{};
  } else if (t == "bc") {
    return AST::DRegister<'b', 'c'>{};
  } else if (t == "de") {
    return AST::DRegister<'d', 'e'>{};
  } else if (t == "hl") {
    return AST::DRegister<'h', 'l'>{};
  } else if (t == "sp") {
    return AST::DRegister<'s', 'p'>{};
  } else if (t == "pc") {
    return AST::DRegister<'p', 'c'>{};
  }
}

AST::Number Parser::parseNumber(Token& t) {
  return static_cast<uint8_t>(std::atoi(t.c_str()));
}

AST::Number Parser::evalNumericOp(TokenList *toks) {
  
}
