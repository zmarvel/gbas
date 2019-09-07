

#include <fstream>

#include "parser.hpp"
#include "elf.hpp"

namespace GBAS {
  enum class SectionType {
    DATA,
    RODATA,
    BSS,
    TEXT,
    INIT,
    INVALID,
  };

  static inline SectionType stringToSectionType(const std::string& str) {
    if (str == ".data") {
      return SectionType::DATA;
    } else if (str == ".rodata") {
      return SectionType::RODATA;
    } else if (str == ".bss") {
      return SectionType::BSS;
    } else if (str == ".text") {
      return SectionType::TEXT;
    } else if (str == ".init") {
      return SectionType::INIT;
    } else {
      return SectionType::INVALID;
    }
  };
}

class AssemblerException : std::exception {
 public:
  AssemblerException(const char* msg) { mMsg = msg; }

  AssemblerException(const std::string& msg) { mMsg = msg; }

  virtual const char* what() const noexcept { return mMsg.c_str(); }

 private:
  std::string mMsg;
};

struct InstructionNone {
  InstructionNone(AST::InstructionType typ) : typ{typ} { }

  constexpr uint8_t encode() {
    using namespace AST;
    uint8_t opcode = 0;
    switch (typ) {
      case InstructionType::NOP:
        opcode = 0x00;
        break;
      case InstructionType::STOP:
        opcode = 0x10;
        break;
      case InstructionType::RLCA:
        opcode = 0x07;
        break;
      case InstructionType::RLA:
        opcode = 0x17;
        break;
      case InstructionType::DAA:
        opcode = 0x27;
        break;
      case InstructionType::SCF:
        opcode = 0x37;
        break;
      case InstructionType::RRCA:
        opcode = 0x0f;
        break;
      case InstructionType::RRA:
        opcode = 0x1f;
        break;
      case InstructionType::CPL:
        opcode = 0x2f;
        break;
      case InstructionType::CCF:
        opcode = 0x3f;
        break;
      case InstructionType::HALT:
        opcode = 0x76;
        break;
      case InstructionType::DI:
        opcode = 0xf3;
        break;
      case InstructionType::RET:
        opcode = 0xc9;
        break;
      case InstructionType::RETI:
        opcode = 0xd9;
        break;
      case InstructionType::EI:
        opcode = 0xfb;
        break;
      default:
        throw AssemblerException{"Unrecognized instruction"};
        break;
    }
    return opcode;
  }

  AST::InstructionType typ;
};

/**
 * The assembler takes an AST as input and outputs an object file. It should
 * evaluate any constant expressions in the AST (trivially optimize).
 */
class Assembler {
 public:
  explicit Assembler();

  /**
   * Parse the AST and put the generated code and symbols into the provided ELF.
   * This function does not write the ELF object to a file.
   *
   * @param ast: AST root node that will be walked.
   * @param elf: ELF object that will be modified to store generated code and
   *   symbols.
   *
   * @throws AssemblerException upon invalid input.
   */
  void assemble(std::shared_ptr<AST::Root> ast, GBAS::ELF& elf);

  GBAS::SectionType currentSectionType() { return mCurrSectionType; }

  /**
   * Helper function to dispatch and generate code for Instruction nodes.
   *
   * @param elf: ELF object containing the "text" section where generated code
   *   will be added.
   * @param instr: BaseInstruction that this function will dispatch and
   *   generate code for.
   *
   * @throws AssemblerException upon invalid instruction input.
   */
  void assembleInstruction(GBAS::ELF& elf, AST::BaseInstruction& instr);

  /**
   * Helper function for assembling instructions with no arguments.
   * 
   * @param instr0: Instruction0 node.
   * 
   * @returns Vector of bytes representing encoded instruction.
   *
   * @throws AssemblerException upon invalid instruction parameter.
   */
  static std::vector<uint8_t> instructionNone(AST::Instruction0& instr0);

  /**
   * Helper function for assembling instructions with one argument, a register.
   * 
   * @param instr1: Instruction1 node.
   * @param reg: Register node--the operand.
   * 
   * @returns Vector of bytes representing encoded instruction.
   *
   * @throws AssemblerException upon invalid instruction parameter.
   */
  static std::vector<uint8_t> instructionR(AST::Instruction1& instr1, const AST::BaseRegister& reg);

  /**
   * Helper function for assembling instructions with one argument, a register.
   * Instructions dispatched to this function operate on the provided register
   * as well as the accumulator register.
   * 
   * @param instr1: Instruction1 node.
   * @param reg: Register node--the operand.
   * 
   * @returns Vector of bytes representing encoded instruction.
   *
   * @throws AssemblerException upon invalid instruction parameter.
   */
  static std::vector<uint8_t> instructionRA(AST::Instruction1& instr1, const AST::BaseRegister& reg);

  /**
   * Helper function for assembling instructions with one argument, a
   * double-register.
   * 
   * @param instr1: Instruction1 node.
   * @param reg: DRegister node--the operand.
   * 
   * @returns Vector of bytes representing encoded instruction.
   *
   * @throws AssemblerException upon invalid instruction parameter.
   */
  static std::vector<uint8_t> instructionD(AST::Instruction1& instr1, const AST::BaseDRegister& reg);

  /**
   * Given any type of node, evaluate it and its descendents, recursively.
   *
   * @returns a deep copy of the optimized node.
   * @throws AssemblerException if any child is invalid.
   */
  static std::shared_ptr<AST::BaseNode> evaluate(
      std::shared_ptr<AST::BaseNode>);

  /**
   * Given a BaseInstruction node, evaluate its operands.
   *
   * @returns a deep copy with child nodes optimized.
   * @throws AssemblerException if any child is invalid.
   */
  static std::shared_ptr<AST::BaseInstruction> evaluateInstruction(
      std::shared_ptr<AST::BaseInstruction> node);

  /**
   * Given a BaseBinaryOp node, evaluate it and its children where possible.
   *
   * @returns a deep copy of the optimized node.
   * @throws AssemblerException if any child is invalid.
   */
  static std::shared_ptr<AST::BaseNode> evaluateBinaryOp(
      std::shared_ptr<AST::BaseBinaryOp> node);

  /**
   * Given a BaseUnaryOp node, evaluate it and its children where possible.
   *
   * @returns a deep copy of the optimized node.
   * @throws AssemblerException if any child is invalid.
   */
  static std::shared_ptr<AST::BaseNode> evaluateUnaryOp(
      std::shared_ptr<AST::BaseUnaryOp> node);

 private:
  GBAS::SectionType mCurrSectionType;
  //std::vector<std::pair<std::string, size_t>> mLabels;
};

