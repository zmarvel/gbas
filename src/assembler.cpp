
#include <algorithm>
#include <map>
#include <memory>

#include "assembler.hpp"
#include "char_utils.hpp"

using namespace AST;
using namespace GBAS;

Assembler::Assembler() { }

void Assembler::assemble(std::shared_ptr<AST::Root> ast, ELF& elf) {
  // This could live on the stack
  for (auto it = ast->begin(); it != ast->end(); it++) {
    auto node = *it;
    switch (node->id()) {
      case NodeType::DIRECTIVE:
        {
          auto directive = std::dynamic_pointer_cast<Directive>(node);
          switch (directive->type()) {
            case DirectiveType::SECTION:
              {
                elf.setSection(directive->operands().at(0));
              }
              break;
            default:
              throw AssemblerException{"Invalid directive type"};
          }
        }
        break;
      case NodeType::INSTRUCTION:
        assembleInstruction(elf,
                            *std::dynamic_pointer_cast<BaseInstruction>(node));
        break;
      case NodeType::LABEL:
        {
          auto label = std::dynamic_pointer_cast<Label>(node);
          size_t value;
          uint8_t info = ELF32_ST_BIND(STB_GLOBAL);
          uint16_t other;
          // TODO support bindings other than GLOBAL
          // TODO add checks for info in ELF
          //switch (mCurrSectionType) {
          //  case SectionType::DATA:
          //    value = elf.dataSize();
          //    info |= ELF32_ST_TYPE(STT_OBJECT);
          //    other = elf.dataIdx();
          //    break;
          //  case SectionType::RODATA:
          //    value = elf.rodataSize();
          //    info |= ELF32_ST_TYPE(STT_OBJECT);
          //    other = elf.rodataIdx();
          //    break;
          //  case SectionType::BSS:
          //    value = elf.bssSize();
          //    info |= ELF32_ST_TYPE(STT_OBJECT);
          //    other = elf.bssIdx();
          //    break;
          //  case SectionType::TEXT:
          //    value = elf.textSize();
          //    info |= ELF32_ST_TYPE(STT_FUNC);
          //    other = elf.textIdx();
          //    break;
          //  case SectionType::INIT:
          //    value = elf.initSize();
          //    info |= ELF32_ST_TYPE(STT_FUNC);
          //    other = elf.initIdx();
          //    break;
          //  default:
          //    throw AssemblerException("Invalid section type");
          //}
          // TODO relocatable
          //elf.addSymbol(label->name(), value, 0, info, STV_DEFAULT, other, true);
          elf.addSymbol(label->name(), value, 0, info, STV_DEFAULT, other, false);
        }
        break;
      default:
        throw AssemblerException("Invalid node");
    }
  }
}

/**
 * Encode the register as a two-bit number. 'm' is a special cheater value for
 * (hl).
 */
constexpr uint8_t encodeRegister(char reg) {
  switch (reg) {
    case 'b':
      return 0;
    case 'c':
      return 1;
    case 'd':
      return 2;
    case 'e':
      return 3;
    case 'h':
      return 4;
    case 'l':
      return 5;
    case 'm':
      return 6;
    case 'a':
      return 7;
    default:
      throw AssemblerException("Invalid register");
  }
}

constexpr uint8_t encodeDRegister(char reg0, char reg1) {
  if (reg0 == 'b' && reg1 == 'c') {
    return 0;
  } else if (reg0 == 'd' && reg1 == 'e') {
    return 1;
  } else if (reg0 == 'h' && reg1 == 'l') {
    return 2;
  } else if (reg0 == 'a' && reg1 == 'f') {
    return 3;
  } else {
    throw AssemblerException("Invalid register");
  }
}

// TODO this works for single-operand ALU instrs but not INC/DEC

/**
 * Instruction with a single register operand:
 *   A ← A op reg
 */
struct InstructionRA {
  InstructionType typ;
  char reg;

  constexpr uint8_t encode() {
    switch (typ) {
      case InstructionType::SUB: {
        uint8_t opcode = 0x12;
        return (opcode << 3) | encodeRegister(reg);
      } break;
      case InstructionType::SBC: {
        uint8_t opcode = 0x13;
        return (opcode << 3) | encodeRegister(reg);
      } break;
      case InstructionType::AND: {
        uint8_t opcode = 0x14;
        return (opcode << 3) | encodeRegister(reg);
      } break;
      case InstructionType::XOR: {
        uint8_t opcode = 0x15;
        return (opcode << 3) | encodeRegister(reg);
      } break;
      case InstructionType::OR: {
        uint8_t opcode = 0x16;
        return (opcode << 3) | encodeRegister(reg);
      } break;
      case InstructionType::CP: {
        uint8_t opcode = 0x17;
        return (opcode << 3) | encodeRegister(reg);
      } break;
      default:
        throw AssemblerException("Invalid InstructionRA type");
    }
  }
};

/**
 * "Unary" instruction operating on a register:
 *   reg ← op reg
 */
struct InstructionR {
  InstructionType typ;
  char reg;

  constexpr uint8_t encode() {
    uint8_t prefix = 0x0;
    switch (typ) {
      case InstructionType::INC: {
        uint8_t opcode = 0x4;
        return (prefix << 6) | encodeRegister(reg) | opcode;
      } break;
      case InstructionType::DEC: {
        uint8_t opcode = 0x5;
        return (prefix << 6) | encodeRegister(reg) | opcode;
      } break;
      default:
        throw AssemblerException("Invalid InstructionR");
    }
  }
};

/**
 * Instruction with a register and an immediate for operands:
 *   reg ← reg op imm8
 */
template <uint8_t prefix, char reg>
struct InstructionRI8 {
  constexpr uint16_t encode() {
    static_assert((prefix & 0xfc) == 0x00);
    return (prefix << 6) | (encodeRegister(reg) << 3) | 0x6;
  }
};

/**
 * Instruction with a dregister for an operand:
 *   dreg ← op dreg
 */
struct InstructionD {
  InstructionType typ;
  char reg0, reg1;
  constexpr uint8_t encode() {
    switch (typ) {
      case InstructionType::INC: {
        uint8_t prefix = 0x0, opcode = 0x3;
        return (prefix << 6) | (encodeDRegister(reg0, reg1) << 4) | opcode;
      } break;
      case InstructionType::DEC: {
        uint8_t prefix = 0x0, opcode = 0xb;
        return (prefix << 6) | (encodeDRegister(reg0, reg1) << 4) | opcode;
      } break;
      case InstructionType::POP: {
        uint8_t prefix = 0x3, opcode = 0x1;
        return (prefix << 6) | (encodeDRegister(reg0, reg1) << 4) | opcode;
      } break;
      case InstructionType::PUSH: {
        uint8_t prefix = 0x3, opcode = 0x5;
        return (prefix << 6) | (encodeDRegister(reg0, reg1) << 4) | opcode;
      } break;
      default:
        throw AssemblerException("Invalid InstructionD");
    }
  }
};

/**
 * Instruction with a (dregister) for an operand.
 */
struct InstructionA {
  InstructionType typ;
  char reg0, reg1;
  constexpr uint8_t encode() {
    if (reg0 != 'h' || reg1 != 'l') {
      throw AssemblerException("Invalid register for InstructionA");
    }
    switch (typ) {
      // TODO gonna have to refactor parser stuff to retain parens so we can
      // differentiate between e.g. INC HL and INC (HL).
      case InstructionType::INC:
        return 0x34;
        break;
      case InstructionType::DEC:
        return 0x35;
        break;
      case InstructionType::JP:
        return 0xe9;
        break;
      default:
        throw AssemblerException("Invalid InstructionA");
    }
  }
};

/**
 * Instruction with a dregister and a 16-bit immediate for operands:
 *   dreg ← dreg op imm16
 */
template <uint8_t prefix, char reg0, char reg1, uint8_t opcode, uint16_t imm>
struct InstructionDI16 {
  constexpr uint32_t encode() {
    static_assert((prefix & 0xfc) == 0x00);
    static_assert((opcode & 0xf0) == 0x00);
    uint8_t instr = (prefix << 6) | (encodeDRegister(reg0, reg1) << 4) | opcode;
    return (instr << 16) | imm;
  }
};

/**
 * Instruction with two register operands:
 *   rd ← rd op rs,
 *   rd ← rs
 */
template <uint8_t prefix, char rd, char rs>
struct InstructionRR {
  constexpr uint8_t encode() {
    static_assert((prefix & 0xfc) == 0);
    return (prefix << 6) | (encodeRegister(rd) << 3) | encodeRegister(rs);
  }
};

/**
 * Instruction with a flag and 8-bit immediate.
 */
template <uint8_t prefix, uint8_t flag, uint8_t opcode, uint8_t imm8>
struct InstructionFI8 {
  constexpr uint8_t encode() {
    static_assert((prefix & 0xfc) == 0);
    static_assert((opcode & 0xc0) == 0);
    return (prefix << 6) | opcode;
  }
};

/**
 * Instruction with an 8-bit immediate.
 */
struct InstructionI8 {
  InstructionType typ;

  constexpr uint8_t encode() {
    switch (typ) {
      case InstructionType::JR:
        return 0x18;
      case InstructionType::SUB:
        return 0xd6;
      case InstructionType::AND:
        return 0xe6;
      case InstructionType::OR:
        return 0xf6;
      case InstructionType::XOR:
        return 0xee;
      case InstructionType::CP:
        return 0xff;
      default:
        throw AssemblerException("Invalid InstructionI8");
    }
  }
};

/**
 * Instruction with an 16-bit immediate.
 */
struct InstructionI16 {
  InstructionType typ;

  constexpr uint8_t encode() {
    switch (typ) {
      case InstructionType::JP:
        return 0xc3;
      case InstructionType::CALL:
        return 0xcd;
      default:
        throw AssemblerException("Invalid InstructionI16");
    }
  }
};

enum class OperandType {
  REGISTER,
  DREGISTER,
  IMM8,
  IMM16,
  RADDR,
  IADDR,
  FLAG,
  INVALID,
};

static const std::map<const InstructionType,
                      const std::vector<std::pair<OperandType, OperandType>>>
  formats{
    // prefix 0x0
    { InstructionType::INC,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::DREGISTER, OperandType::INVALID },
      } },
    { InstructionType::DEC,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::DREGISTER, OperandType::INVALID },
      } },
    { InstructionType::LD,
      {
        { OperandType::REGISTER, OperandType::IMM8 },
        { OperandType::DREGISTER, OperandType::IMM16 },
      } },
    { InstructionType::NOP,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::STOP,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::RLCA,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::RLA,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::DAA,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::SCF,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::RRCA,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::RRA,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::CPL,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::CCF,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::JR,
      {
        { OperandType::FLAG, OperandType::IMM8 },
        { OperandType::IMM8, OperandType::INVALID },
      } },
    { InstructionType::HALT,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::SUB,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::REGISTER, OperandType::IMM8 },
      } },
    { InstructionType::AND,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::REGISTER, OperandType::IMM8 },
      } },
    { InstructionType::XOR,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::REGISTER, OperandType::IMM8 },
      } },
    { InstructionType::OR,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::REGISTER, OperandType::IMM8 },
      } },
    { InstructionType::CP,
      {
        { OperandType::REGISTER, OperandType::INVALID },
        { OperandType::REGISTER, OperandType::IMM8 },
      } },
    { InstructionType::POP,
      { { OperandType::DREGISTER, OperandType::INVALID } } },
    { InstructionType::PUSH,
      { { OperandType::DREGISTER, OperandType::INVALID } } },
    { InstructionType::DI, { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::EI, { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::RET,
      {
        { OperandType::INVALID, OperandType::INVALID },
        { OperandType::FLAG, OperandType::IMM8 },
      } },
    { InstructionType::RETI,
      { { OperandType::INVALID, OperandType::INVALID } } },
    { InstructionType::JP,
      {
        { OperandType::FLAG, OperandType::IMM8 },
        { OperandType::IMM16, OperandType::INVALID },
      } },
    { InstructionType::CALL,
      {
        { OperandType::FLAG, OperandType::IMM8 },
        { OperandType::IMM16, OperandType::INVALID },
      } },
  };

// TODO
// jp (hl)

// prefix 0x0 TODO:
// jr <flag>, imm8
// ld (DR[+/-]), a
// ld (imm16), sp
// add HL, DR
// ld a, (DR[+/-])

void Assembler::assembleInstruction(ELF& elf, BaseInstruction& instr) {
  switch (instr.nOperands()) {
    case 0: {
      auto& instr0 = dynamic_cast<Instruction0&>(instr);
      const auto& fmts = formats.find(instr0.type());
      if (fmts == formats.end()) {
        // didn't find the key
      } else {
        if (std::any_of(fmts->second.begin(), fmts->second.end(), [](auto fmt) {
              return (fmt.first == OperandType::INVALID) &&
                     (fmt.second == OperandType::INVALID);
            })) {
          elf.addProgbits(std::vector<uint8_t>{InstructionNone{instr0.type()}.encode()});
        } else {
          throw AssemblerException("Invalid instruction0 usage");
        }
      }
    } break;

    case 1: {
      auto& instr1 = dynamic_cast<Instruction1&>(instr);
      const auto& fmts = formats.find(instr1.type());
      if (fmts == formats.end()) {
        // didn't find the key
        throw AssemblerException("Invalid instruction format");
      } else {
        // search formats for one that matches this instruction
        auto toperand = instr1.operand()->id();
        for (auto it = fmts->second.begin(); it != fmts->second.end(); it++) {
          if (it->second != OperandType::INVALID) {
            continue;
          } else if (toperand == NodeType::REGISTER &&
                     it->first == OperandType::REGISTER) {
            auto reg =
                std::dynamic_pointer_cast<BaseRegister>(instr1.operand());
            switch (instr1.type()) {
              case InstructionType::INC:
              case InstructionType::DEC:
                elf.addProgbits(instructionR(instr1, *reg));
                return;
                break;
              case InstructionType::SUB:
              case InstructionType::SBC:
              case InstructionType::AND:
              case InstructionType::XOR:
              case InstructionType::OR:
              case InstructionType::CP:
                elf.addProgbits(instructionRA(instr1, *reg));
                return;
                break;
              default:
                throw AssemblerException("Invalid Instruction1");
            }
          } else if (toperand == NodeType::DREGISTER &&
                     it->first == OperandType::DREGISTER) {
            auto reg =
                std::dynamic_pointer_cast<BaseDRegister>(instr1.operand());
            switch (instr1.type()) {
              case InstructionType::INC:
              case InstructionType::DEC:
              case InstructionType::POP:
              case InstructionType::PUSH:
                elf.addProgbits(instructionD(instr1, *reg));
                return;
                break;
              default:
                throw AssemblerException("Invalid Instruction1");
            }
          } else if (toperand == NodeType::DREGISTER &&
                     it->first == OperandType::RADDR) {
            auto reg =
                std::dynamic_pointer_cast<BaseDRegister>(instr1.operand());
            switch (instr1.type()) {
              case InstructionType::INC:
              case InstructionType::DEC:
              case InstructionType::JP: {
                std::vector<uint8_t> encoded{
                    InstructionA{instr1.type(), reg->reg1(), reg->reg2()}
                        .encode()};
                elf.addProgbits(encoded);
              }
                return;
                break;
              default:
                throw AssemblerException("Invalid Instruction1");
            }
          } else if (toperand == NodeType::NUMBER) {
            switch (it->first) {
              case OperandType::IMM8:
                switch (instr1.type()) {
                  case InstructionType::JR:
                  case InstructionType::SUB:
                  case InstructionType::AND:
                  case InstructionType::OR:
                  case InstructionType::XOR:
                  case InstructionType::CP: {
                    auto num =
                        std::dynamic_pointer_cast<Number>(instr1.operand());
                    std::vector<uint8_t> encoded{
                        InstructionI8{instr1.type()}.encode(), num->value()};
                  } break;
                  default:
                    throw AssemblerException("Invalid Instruction1");
                }
                break;
              case OperandType::IMM16:
                switch (instr1.type()) {
                  case InstructionType::JP:
                  case InstructionType::CALL: {
                    auto num =
                        std::dynamic_pointer_cast<Number>(instr1.operand());
                    // TODO is it little-endian?
                    uint8_t hi = num->value() >> 8, lo = num->value() & 0xff;
                    std::vector<uint8_t> encoded{
                        InstructionI16{instr1.type()}.encode(), hi, lo};
                  } break;
                  default:
                    throw AssemblerException("Invalid Instruction1");
                }
                break;
              default:
                continue;
            }
          } else if (toperand == NodeType::LABEL &&
              it->first == OperandType::IMM8) {
            // TODO
          } else if (toperand == NodeType::LABEL &&
              it->first == OperandType::IMM16) {
            // TODO
          } else if (toperand == NodeType::LABEL &&
              it->first == OperandType::IADDR) {
            // TODO
            // operand could be a label--handle relocation
          }
        }
        throw AssemblerException("Invalid instruction1 usage");
      }
    }

    break;

    case 2:
      break;
  }
}

std::vector<uint8_t> Assembler::instructionR(Instruction1& instr1,
                                             const BaseRegister& reg) {
  switch (instr1.type()) {
    case InstructionType::INC:
      return std::vector<uint8_t>{
          InstructionR{InstructionType::INC, reg.reg()}.encode()};
    case InstructionType::DEC:
      return std::vector<uint8_t>{
          InstructionR{InstructionType::DEC, reg.reg()}.encode()};
    default:
      throw AssemblerException("Unrecognized Instruction0 type");
  }
}

std::vector<uint8_t> Assembler::instructionRA(Instruction1& instr1,
                                              const BaseRegister& reg) {
  switch (instr1.type()) {
    case InstructionType::SUB:
      return std::vector<uint8_t>{
          InstructionRA{InstructionType::SUB, reg.reg()}.encode()};
    case InstructionType::SBC:
      return std::vector<uint8_t>{
          InstructionRA{InstructionType::SBC, reg.reg()}.encode()};
    case InstructionType::AND:
      return std::vector<uint8_t>{
          InstructionRA{InstructionType::AND, reg.reg()}.encode()};
    case InstructionType::XOR:
      return std::vector<uint8_t>{
          InstructionRA{InstructionType::XOR, reg.reg()}.encode()};
    case InstructionType::OR:
      return std::vector<uint8_t>{
          InstructionRA{InstructionType::OR, reg.reg()}.encode()};
    case InstructionType::CP:
      return std::vector<uint8_t>{
          InstructionRA{InstructionType::CP, reg.reg()}.encode()};
    default:
      throw AssemblerException("Invalid InstructionRA type");
  }
}

std::vector<uint8_t> Assembler::instructionD(Instruction1& instr1,
                                             const BaseDRegister& reg) {
  switch (instr1.type()) {
    case InstructionType::INC:
      return std::vector<uint8_t>{
          InstructionD{InstructionType::INC, reg.reg1(), reg.reg2()}.encode()};
      break;
    case InstructionType::DEC:
      return std::vector<uint8_t>{
          InstructionD{InstructionType::DEC, reg.reg1(), reg.reg2()}.encode()};
    case InstructionType::POP:
      return std::vector<uint8_t>{
          InstructionD{InstructionType::POP, reg.reg1(), reg.reg2()}.encode()};
    case InstructionType::PUSH:
      return std::vector<uint8_t>{
          InstructionD{InstructionType::PUSH, reg.reg1(), reg.reg2()}.encode()};
    default:
      throw AssemblerException("Invalid Instruction1");
  }
}

std::shared_ptr<BaseNode> Assembler::evaluate(std::shared_ptr<BaseNode> node) {
  switch (node->id()) {
    case NodeType::ROOT: {
      auto root = std::dynamic_pointer_cast<Root>(node);
      auto newRoot = std::make_shared<Root>();
      for (auto it = root->begin(); it != root->end(); it++) {
        newRoot->add(evaluate(*it));
      }
      return newRoot;
    } break;
    case NodeType::INSTRUCTION: {
      auto instr = std::dynamic_pointer_cast<BaseInstruction>(node);
      return evaluateInstruction(instr);
    } break;
    case NodeType::REGISTER: {
      auto reg = std::dynamic_pointer_cast<BaseRegister>(node);
      // Can't evaluate a register.
      return reg;
    } break;
    case NodeType::DREGISTER: {
      auto reg = std::dynamic_pointer_cast<BaseDRegister>(node);
      return reg;
    } break;
    case NodeType::LABEL: {
      auto label = std::dynamic_pointer_cast<Label>(node);
      // Can't evaluate a label until link-time.
      return label;
    } break;
    case NodeType::NUMBER: {
      auto num = std::dynamic_pointer_cast<Number>(node);
      // A number's value is just itself.
      return num;
    } break;
    case NodeType::BINARY_OP: {
      auto op = std::dynamic_pointer_cast<BaseBinaryOp>(node);
      return evaluateBinaryOp(op);
    } break;
    case NodeType::UNARY_OP: {
      auto op = std::dynamic_pointer_cast<BaseUnaryOp>(node);
      return evaluateUnaryOp(op);
    } break;
    case NodeType::INVALID:
    default:
      throw AssemblerException("Unrecognized AST node type");
  }
}

std::shared_ptr<BaseInstruction> Assembler::evaluateInstruction(
    std::shared_ptr<BaseInstruction> node) {
  switch (node->nOperands()) {
    case 0:
      return node;
      break;
    case 1: {
      // auto instr1 = std::dynamic_pointer_cast<Instruction1>(node);
      auto instr1 = std::dynamic_pointer_cast<Instruction1>(node);
      return std::make_shared<Instruction1>(instr1->type(),
                                            evaluate(instr1->operand()));
    } break;
    case 2: {
      auto instr2 = std::dynamic_pointer_cast<Instruction2>(node);
      return std::make_shared<Instruction2>(
          instr2->type(), evaluate(instr2->left()), evaluate(instr2->right()));
    } break;
    default:
      throw AssemblerException("Invalid number of Instruction operands");
  }
}

std::shared_ptr<BaseNode>
Assembler::evaluateBinaryOp(std::shared_ptr<BaseBinaryOp> node)
{
  switch (node->opType()) {
    case BinaryOpType::ADD: {
      auto op = std::dynamic_pointer_cast<AddOp>(node);
      auto vLeft = evaluate(op->left());
      auto vRight = evaluate(op->right());
      if (vLeft->id() == NodeType::NUMBER && vRight->id() == NodeType::NUMBER) {
        auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
        auto rnum = std::dynamic_pointer_cast<Number>(vRight);
        return std::make_shared<Number>(lnum->value() + rnum->value());
      } else {
        // Can't evaluate anything
        return std::make_shared<AddOp>(vLeft, vRight);
      }
    } break;
    case BinaryOpType::SUB: {
      auto op = std::dynamic_pointer_cast<SubOp>(node);
      auto vLeft = evaluate(op->left());
      auto vRight = evaluate(op->right());
      if (vLeft->id() == NodeType::NUMBER && vRight->id() == NodeType::NUMBER) {
        auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
        auto rnum = std::dynamic_pointer_cast<Number>(vRight);
        return std::make_shared<Number>(lnum->value() - rnum->value());
      } else {
        // Can't evaluate anything
        return std::make_shared<SubOp>(vLeft, vRight);
      }
    } break;
    case BinaryOpType::MULT: {
      auto op = std::dynamic_pointer_cast<MultOp>(node);
      auto vLeft = evaluate(op->left());
      auto vRight = evaluate(op->right());
      if (vLeft->id() == NodeType::NUMBER && vRight->id() == NodeType::NUMBER) {
        auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
        auto rnum = std::dynamic_pointer_cast<Number>(vRight);
        return std::make_shared<Number>(lnum->value() * rnum->value());
      } else {
        // Can't evaluate anything
        return std::make_shared<MultOp>(vLeft, vRight);
      }
    } break;
    case BinaryOpType::DIV: {
      auto op = std::dynamic_pointer_cast<DivOp>(node);
      auto vLeft = evaluate(op->left());
      auto vRight = evaluate(op->right());
      if (vLeft->id() == NodeType::NUMBER && vRight->id() == NodeType::NUMBER) {
        auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
        auto rnum = std::dynamic_pointer_cast<Number>(vRight);
        return std::make_shared<Number>(lnum->value() / rnum->value());
      } else {
        // Can't evaluate anything
        return std::make_shared<DivOp>(vLeft, vRight);
      }
    } break;
    default:
      throw AssemblerException("Invalid BinaryOpType");
  }
}

std::shared_ptr<BaseNode> Assembler::evaluateUnaryOp(
    std::shared_ptr<BaseUnaryOp> node) {
  switch (node->opType()) {
    case UnaryOpType::NEG: {
      auto op = std::dynamic_pointer_cast<NegOp>(node);
      auto vRand = evaluate(op->operand());
      if (vRand->id() == NodeType::NUMBER) {
        auto num = std::dynamic_pointer_cast<Number>(vRand);
        return std::make_shared<Number>(-num->value());
      } else {
        // Can't evaluate anything
        return std::make_shared<NegOp>(vRand);
      }
    } break;
    default:
      throw AssemblerException("Invalid UnaryOpType");
  }
}
