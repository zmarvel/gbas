
#include <algorithm>

#include "parser.hpp"

Parser::Parser(TokenList& tokens) : mTokens{tokens}, mRoot{}, mPos{0} {}

Parser::~Parser() {}

using namespace AST;

static const std::array<const Token, 8> registers = {
    "a", "f", "b", "c", "d", "e", "h", "l",
};

static const std::array<const Token, 6> doubleRegisters = {
    "af", "bc", "de", "hl", "sp", "pc",
};

static InstructionPropsList instructions{{
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
    {"ret", InstructionType::RET, 0, 0},
    {"jp", InstructionType::JP, 0, 0},
    {"call", InstructionType::CALL, 0, 0},
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
}};

Token Parser::next() { return mTokens.at(mPos++); }

Token Parser::peek() {
  if (static_cast<size_t>(mPos) < mTokens.size()) {
    return mTokens.at(mPos);
  } else {
    // Assume no spaces will have survived tokenization. This is admittedly a
    // hack ;)
    return " ";
  }
}

Token Parser::peekNext() { return mTokens.at(mPos + 1); }

std::shared_ptr<BaseNode> Parser::parse() { return program(); };

std::shared_ptr<BaseNode> Parser::program() {
  std::vector<std::shared_ptr<BaseNode>> lines;
  while (!isEof(peek())) {
    lines.push_back(line());
  }
  return std::make_shared<Root>(lines);
}

std::shared_ptr<BaseNode> Parser::line() {
  auto tok = peek();
  if (isLabel(tok) && (peekNext() == ":")) {
    return label();
    // TODO
    // } else if (isDirective(tok)) {
  } else if (isInstruction(tok)) {
    return instruction();
  } else {
    throw ParserException("Invalid token in program");
  }
}

std::shared_ptr<BaseNode> Parser::label() {
  auto tok = next();
  if (isLabel(tok)) {
    return std::make_shared<Label>(tok);
  } else {
    throw ParserException("Invalid label");
  }
}

std::shared_ptr<BaseNode> Parser::instruction() {
  auto inst = next();
  std::vector<std::shared_ptr<BaseNode>> operands;
  while (!isNewline(peek()) && operands.size() < 3) {
    operands.push_back(operand());
  }
  auto props = findInstruction(inst);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (operands.size() == static_cast<size_t>(props->args1) ||
             operands.size() == static_cast<size_t>(props->args2)) {
    switch (operands.size()) {
      case 0:
        return std::make_shared<Instruction0>(props->type);
      case 1:
        return std::make_shared<Instruction1>(props->type, operands.at(0));
      case 2:
        return std::make_shared<Instruction2>(props->type, operands.at(0),
                                              operands.at(1));
      default:
        throw ParserException("Invalid number of args for instruction");
    }
  } else {
    throw ParserException("Invalid number of args for instruction");
  }
}

std::shared_ptr<BaseNode> Parser::operand() {
  auto tok = peek();
  if (isRegister(tok)) {
    return register_();
  } else if (isDRegister(tok)) {
    return dregister();
  } else {
    return addition();
  }
}

std::shared_ptr<BaseNode> Parser::register_() {
  auto tok = next();
  if (tok.size() != 1) {
    throw ParserException("Unrecognized Register");
  }

  switch (tok.at(0)) {
    case 'a':
      return std::make_shared<Register<'a'>>();
    case 'f':
      return std::make_shared<Register<'f'>>();
    case 'b':
      return std::make_shared<Register<'b'>>();
    case 'c':
      return std::make_shared<Register<'c'>>();
    case 'd':
      return std::make_shared<Register<'d'>>();
    case 'e':
      return std::make_shared<Register<'e'>>();
    case 'h':
      return std::make_shared<Register<'h'>>();
    case 'l':
      return std::make_shared<Register<'l'>>();
    default:
      throw ParserException("Unrecognized Register");
  }
}

std::shared_ptr<BaseNode> Parser::dregister() {
  auto tok = next();
  if (tok == "af") {
    return std::make_shared<DRegister<'a', 'f'>>();
  } else if (tok == "bc") {
    return std::make_shared<DRegister<'b', 'c'>>();
  } else if (tok == "de") {
    return std::make_shared<DRegister<'d', 'e'>>();
  } else if (tok == "hl") {
    return std::make_shared<DRegister<'h', 'l'>>();
  } else if (tok == "sp") {
    return std::make_shared<DRegister<'s', 'p'>>();
  } else if (tok == "pc") {
    return std::make_shared<DRegister<'p', 'c'>>();
  } else {
    throw ParserException("Unrecognized DRegister");
  }
}

std::shared_ptr<BaseNode> Parser::addition() {
  auto left = multiplication();
  while (isAddition(peek())) {
    auto op = next().at(0);
    switch (op) {
      case '+':
        left = std::make_shared<AddOp>(left, multiplication());
        break;
      case '-':
        left = std::make_shared<SubOp>(left, multiplication());
        break;
    }
  }
  return left;
}

bool Parser::isAddition(const Token& tok) {
  return (tok.size() > 0) && ((tok == "+") || (tok == "-"));
}

std::shared_ptr<BaseNode> Parser::multiplication() {
  auto left = unary();
  while (isMultiplication(peek())) {
    auto op = next().at(0);
    switch (op) {
      case '*':
        left = std::make_shared<MultOp>(left, unary());
        break;
      case '/':
        left = std::make_shared<DivOp>(left, unary());
        break;
    }
  }
  return left;
}

bool Parser::isMultiplication(const Token& tok) {
  return (tok.size() > 0) && ((tok == "*") || (tok == "/"));
}

std::shared_ptr<BaseNode> Parser::unary() {
  if (peek().size() < 1) {
    throw ParserException("Invalid unary op");
  }
  switch (peek().at(0)) {
    case '-':
      next();
      return std::make_shared<NegOp>(unary());
    default:
      return primary();
  }
}

std::shared_ptr<BaseNode> Parser::primary() {
  auto tok = peek();
  if (isLabel(tok)) {
    return label();
  } else if (isNumber(tok)) {
    return number();
  } else {
    throw ParserException("Unrecognized primary expression");
  }
}

std::shared_ptr<BaseNode> Parser::number() {
  return std::make_shared<Number>(
      static_cast<Number>(std::atoi(next().c_str())));
}

bool Parser::isNewline(const Token& tok) { return tok == "EOL"; }

int Parser::expectNewline(const TokenList& list, int start, int max) {
  for (int i = start; i < max; i++) {
    if (isNewline(list.at(i))) {
      return i;
    }
  }
  return -1;
}

bool Parser::isEof(const Token& tok) { return tok == "EOF"; }

TokenList Parser::readLine(TokenList& tokens) {
  TokenList list{};
  for (int i = 0; tokens.at(i) != "EOL"; i++) {
    list.push_back(tokens.at(i));
  }
  return list;
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
    if ((*it < '0' || *it > '9') && (*it < 'A' || *it > 'Z') &&
        (*it < 'a' || *it > 'z') && *it != '_') {
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
