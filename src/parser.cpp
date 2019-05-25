
#include <algorithm>

#include "parser.h"

Parser::Parser(TokenList& tokens) :
  mTokens{tokens},
  mRoot{},
  mPos{0}
{ }

Parser::~Parser() {

}


using namespace AST;

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
} };



Token Parser::next() {
  return mTokens.at(mPos++);
}

Token Parser::peek() {
  if (static_cast<size_t>(mPos) < mTokens.size()) {
    return mTokens.at(mPos);
  } else {
    // Assume no spaces will have survived tokenization. This is admittedly a
    // hack ;)
    return " ";
  }
}

Token Parser::peekNext() {
  return mTokens.at(mPos+1);
}

std::shared_ptr<BaseNode> Parser::parse() {
  return program();
};

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
  return std::make_shared<Label>(next());
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
  return (tok == "+") || (tok == "-");
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
  return (tok == "*") || (tok == "/");
}

std::shared_ptr<BaseNode> Parser::unary() {
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
  return std::make_shared<Number>(static_cast<Number>(std::atoi(next().c_str())));
}


bool Parser::isNewline(const Token& tok) {
  return tok == "EOL";
}

int Parser::expectNewline(const TokenList& list, int start, int max) {
  for (int i = start; i < max; i++) {
    if (isNewline(list.at(i))) {
      return i;
    }
  }
  return -1;
}

bool Parser::isEof(const Token& tok) {
  return tok == "EOF";
}

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

#if 0

std::shared_ptr<Root> Parser::parse(TokenList& tokens, int pos, int line) {
  //int i = 0;
  Token& currTok = tokens.at(pos);
  // TODO: only label definitions and instructions should start a line
  if (isEof(currTok)) {
    return std::shared_ptr<Root>{&mRoot};
  } else if (isNewline(currTok)) {
    return parse(tokens, pos + 1, line + 1);
  } else if (isNumber(currTok)) {
    std::shared_ptr<BaseNode> numl = parseNumber(currTok);
    if (isNumericOp(tokens.at(pos+1))) {
      std::shared_ptr<BaseNode> op = parseNumericOp(numl, tokens.at(pos+1),
                                                    parseNumber(tokens.at(pos+2)));
      mRoot.add(*op);
      //i += 3;
      return parse(tokens, pos + 3, line);
    } else {
      mRoot.add(*numl);
      //i++;
      return parse(tokens, pos + 1, line);
    }
  } else if (isLabel(currTok)) {
      mRoot.add(*parseLabel(currTok));
      return parse(tokens, pos + 1, line);
  } else {
    // How many operands does it have?
    // Based on that we can look ahead.
    auto props = findInstruction(currTok);
    if (props != instructions.end()) {
      TokenList operands = readLine(tokens, pos + 1);
      mRoot.add(*parseInstruction(currTok, operands));
      return parse(tokens, pos + 1 + operands.size(), line);
    } else {
      throw ParserException("Unrecognized instruction");
    }

    return std::shared_ptr<Root>{&mRoot};
  }
}

std::shared_ptr<AST::RegisterBase> Parser::parseRegister(const Token& tok) {
  if (tok.size() != 1) {
    throw ParserException("Unrecognized Register");
  }
  switch (tok.at(0)) {
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


std::shared_ptr<BaseNode> Parser::parseNumericOp(std::shared_ptr<BaseNode> lrand,
                                                 const Token& t,
                                                 std::shared_ptr<BaseNode> rrand) {
  if ((lrand->id() != NodeType::NUMBER && lrand->id() != NodeType::NUMERIC_OP) ||
      (rrand->id() != NodeType::NUMBER && rrand->id() != NodeType::NUMERIC_OP)) {
    throw ParserException("Invalid NumericOp operand types");
  } else {
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
}

std::shared_ptr<BaseNode> Parser::parseNumericOp(const TokenList& toks) {
  auto tok = toks.begin();
  while (tok != toks.end()) {
  
    std::shared_ptr<BaseNode> left;
    else if (isNumber(*tok)) {
      auto left = parseNumber(*tok);
      tok++;
      if (isNumericOp(*tok)) {
        auto opTok = *tok;
        auto right = parseNumber(*tok);
        return parseNumericOp(left, opTok, right);
      } else {
        return left;
      }
  } else if (isLabel(*tok)) {
    auto left = parseLabel(*tok);
    tok++;
  }
  

  if ((lrand->id() != NodeType::NUMBER && lrand->id() != NodeType::NUMERIC_OP) ||
      (rrand->id() != NodeType::NUMBER && rrand->id() != NodeType::NUMERIC_OP)) {
    throw ParserException("Invalid NumericOp operand types");
  } else {
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

std::shared_ptr<BaseNode> Parser::parseInstruction(const Token& tok,
                                                   std::shared_ptr<BaseNode> rand) {
  auto props = findInstruction(tok);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (props->args1 != 1 && props->args2 != 1) {
    throw ParserException("Too many arguments for instruction");
  }

  return std::make_shared<Instruction1>(props->type, rand);
}

std::shared_ptr<BaseNode> Parser::parseInstruction(const Token& tok,
                                                   std::shared_ptr<BaseNode> rand1,
                                                   std::shared_ptr<BaseNode> rand2) {
  auto props = findInstruction(tok);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (props->args1 != 2 && props->args2 != 2) {
    throw ParserException("Too many arguments for instruction");
  }

  return std::make_shared<Instruction2>(props->type, rand1, rand2);
}

std::shared_ptr<BaseNode> Parser::parseInstruction(const Token& tok,
                                                   const TokenList& rands) {
  auto props = findInstruction(tok);
  if (props == instructions.end()) {
    throw ParserException("Unrecognized instruction");
  } else if (props->args1 != 0 && props->args2 != 0) {
    throw ParserException("Too many arguments for instruction");
  }

  return std::make_shared<Instruction0>(props->type);
}


std::shared_ptr<BaseNode> Parser::parseLabel(const Token& tok) {
  if (!isLabel(tok)) {
    throw ParserException("Invalid label");
  }
  return std::make_shared<Label>(tok);
}

std::shared_ptr<BaseNode> Parser::parseOperands(const TokenList& toks) {
  auto tok = toks.begin();
  while (tok != toks.end()) {
    if (isRegister(*tok)) {
      return parseRegister(*tok);
    } else if (isNumber(*tok)) {
      auto left = parseNumber(*tok);
      tok++;
      if (isNumericOp(*tok)) {
        auto opTok = *tok;
        auto right = parseNumber(*tok);
        return parseNumericOp(left, opTok, right);
      } else {
        return left;
      }
    } else if (isLabel(*tok)) {
      return parseLabel(*tok);
    } else {
      throw ParserException("Unable to determine operand type");
    }
  }
}


#endif
