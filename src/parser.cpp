
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

static InstructionPropsList instructions{ {
  {"add", InstructionType::ADD, 2, 2},
  {"adc", InstructionType::ADC, 2, 2},
  {"inc", InstructionType::INC, 1, 1},
  {"sub", InstructionType::SUB, 1, 2},
  {"sbc", InstructionType::SBC, 2, 2},
  {"and", InstructionType::AND, 1, 1},
  {"xor", InstructionType::XOR, 1, 1},
  {"or", InstructionType::OR, 1, 1},
  {"cp", InstructionType::CP, 1, 1},
  {"dec", InstructionType::DEC, 1, 1},
  {"rlc", InstructionType::RLC, 1, 1},
  {"rlca", InstructionType::RLCA, 0, 0},
  {"rl", InstructionType::RL, 1, 1},
  {"rla", InstructionType::RLA, 0, 0},
  {"rrc", InstructionType::RRC, 1, 1},
  {"rrca", InstructionType::RRCA, 0, 0},
  {"rr", InstructionType::RR, 1, 1},
  {"rra", InstructionType::RRA, 0, 0},
  {"daa", InstructionType::DAA, 0, 0},
  {"scf", InstructionType::SCF, 0, 0},
  {"cpl", InstructionType::CPL, 0, 0},
  {"ccf", InstructionType::CCF, 0, 0},

  {"ld", InstructionType::LD, 2, 2},
  {"ldi", InstructionType::LDI, 2, 2},
  {"ldd", InstructionType::LDD, 2, 2},
  {"push", InstructionType::PUSH, 1, 1},
  {"pop", InstructionType::POP, 1, 1},

  {"jr", InstructionType::JR, 1, 2},
  {"ret", InstructionType::RET, 0},
  {"jp", InstructionType::JP, 0},
  {"call", InstructionType::CALL, 0},
  {"rst", InstructionType::RST, 1, 1},

  {"nop", InstructionType::NOP, 0, 0},
  {"stop", InstructionType::STOP, 0, 0},
  {"halt", InstructionType::HALT, 0, 0},
  {"di", InstructionType::DI, 0, 0},
  {"ei", InstructionType::EI, 0, 0},

  {"sla", InstructionType::SLA, 1, 1},
  {"sra", InstructionType::SRA, 1, 1},
  {"swap", InstructionType::SWAP, 1, 1},
  {"srl", InstructionType::SRL, 1, 1},
  {"bit", InstructionType::BIT, 2, 2},
  {"res", InstructionType::RES, 2, 2},
  {"set", InstructionType::SET, 2, 2},
} };


using namespace AST;

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


std::shared_ptr<BaseNode> Parser::parse(TokenList& tokens, int i, BaseNode prev) {
  Token& currTok = tokens.at(i);
  if (isNumber(currTok)) {
    return parseNumber(currTok);
  } else if (isNumericOp(currTok)) {
    std::shared_ptr<BaseNode> rrand = parse(tokens, i+1);
    return parseNumericOp(prev, currTok, *rrand);
  } else if (isLabel(currTok)) {
    //BaseNode label = parseLabel(currTok);
    //return label;
  } else if (isInstruction(currTok)) {
    // How many operands does it have?
    // Based on that we can look ahead.
  }
  return std::make_shared<Root>();
}

bool Parser::isNumber(const Token& tok) {
  if (tok.size() == 0) {
    return false;
  }

  for (auto it = tok.begin(); it != tok.end(); it++) {
    if (*it < '0' || *it > '9') {
      return false;
    }
  }
  return true;
}

bool Parser::isNumericOp(const Token& tok) {
  if (tok == "+") {
    return true;
  } else if (tok == "-") {
    return true;
  } else if (tok == "*") {
    return true;
  } else if (tok == "/") {
    return true;
  } else {
    return false;
  }
}

InstructionPropsList::const_iterator Parser::findInstruction(const Token& tok) {
  for (auto it = instructions.begin(); it != instructions.end(); it++) {
    if (it->lexeme == tok) {
      return it;
    }
  }
  return instructions.end();
}

bool Parser::isInstruction(const Token& tok) {
  return findInstruction(tok) != instructions.end();
}

bool Parser::isLabel(const Token& tok) {
  if (tok.size() == 0) {
    return false;
  } else if (tok[0] >= '0' && tok[0] <= '9') {
    // Labels can't start with a number
    return false;
  }

  for (auto it = tok.begin(); it != tok.end(); it++) {
    if ((*it < '0' || *it > '9') &&
        (*it < 'A' || *it > 'Z') && (*it < 'a' || *it > 'z') &&
        *it != '_') {
      return false;
    }
  }

  if (findInstruction(tok) != instructions.end()) {
    return false;
  }

  return true;
}

bool Parser::isRegister(const Token& tok) {
  return std::find(registers.begin(), registers.end(), tok) != registers.end();
}

bool Parser::isDRegister(const Token& tok) {
  return std::find(doubleRegisters.begin(), doubleRegisters.end(), tok) !=
    doubleRegisters.end();
}

std::shared_ptr<BaseNode> Parser::parseRegister(const Token& t) {
  if (t.size() != 1) {
    throw ParserException("Unrecognized Register");
  }
  switch (t.at(0)) {
    case 'a': return std::make_shared<Register<'a'>>();
    case 'f': return std::make_shared<Register<'f'>>();
    case 'b': return std::make_shared<Register<'b'>>();
    case 'c': return std::make_shared<Register<'c'>>();
    case 'd': return std::make_shared<Register<'d'>>();
    case 'e': return std::make_shared<Register<'e'>>();
    case 'h': return std::make_shared<Register<'h'>>();
    case 'l': return std::make_shared<Register<'l'>>();
    default: throw ParserException("Unrecognized Register");
  }
}


std::shared_ptr<DRegisterBase> Parser::parseDRegister(const Token& t) {
  if (t.size() != 2) {
    throw ParserException("Unrecognized Register");
  }
  if (t == "af") {
    return std::make_shared<DRegister<'a', 'f'>>();
  } else if (t == "bc") {
    return std::make_shared<DRegister<'b', 'c'>>();
  } else if (t == "de") {
    return std::make_shared<DRegister<'d', 'e'>>();
  } else if (t == "hl") {
    return std::make_shared<DRegister<'h', 'l'>>();
  } else if (t == "sp") {
    return std::make_shared<DRegister<'s', 'p'>>();
  } else if (t == "pc") {
    return std::make_shared<DRegister<'p', 'c'>>();
  } else {
    throw ParserException("Unrecognized DRegister");
  }
}

std::shared_ptr<BaseNode> Parser::parseNumber(const Token& t) {
  return std::make_shared<Number>(static_cast<Number>(std::atoi(t.c_str())));
}


std::shared_ptr<BaseNode> Parser::parseNumericOp(BaseNode lrand, const Token& t, BaseNode rrand) {
  if (t == "+") {
    return std::make_shared<NumericOp<BinaryOp::ADD>>(lrand, rrand);
  } else if (t == "-") {
    return std::make_shared<NumericOp<BinaryOp::SUB>>(lrand, rrand);
  } else if (t == "*") {
    return std::make_shared<NumericOp<BinaryOp::MULT>>(lrand, rrand);
  } else if (t == "/") {
    return std::make_shared<NumericOp<BinaryOp::DIV>>(lrand, rrand);
  } else {
    throw ParserException("Unrecognized NumericOp token");
  }
}

std::shared_ptr<BaseNode> Parser::parseInstruction(const Token& tok) {
  auto props = findInstruction(tok);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (props->args1 != 0 && props->args2 != 0) {
    throw ParserException("Too many arguments for instruction");
  }

  return std::make_shared<Instruction0>(props->type);
}

std::shared_ptr<BaseNode> Parser::parseInstruction(const Token& tok, BaseNode rand) {
  auto props = findInstruction(tok);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (props->args1 != 1 && props->args2 != 1) {
    throw ParserException("Too many arguments for instruction");
  }

  return std::make_shared<Instruction1>(props->type, rand);
}

std::shared_ptr<BaseNode> Parser::parseInstruction(const Token& tok, BaseNode lrand, BaseNode rrand) {
  auto props = findInstruction(tok);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (props->args1 != 2 && props->args2 != 2) {
    throw ParserException("Too many arguments for instruction");
  }

  return std::make_shared<Instruction2>(props->type, lrand, rrand);
}


