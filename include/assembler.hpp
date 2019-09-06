

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
  };
}

/**
 * The assembler takes an AST as input and outputs an object file. It should
 * evaluate any constant expressions in the AST (trivially optimize).
 */
class Assembler {
 public:
  explicit Assembler(std::shared_ptr<AST::Root> ast, std::ofstream out);

  void assemble();

  void assembleInstruction(GBAS::ELF& elf, AST::BaseInstruction& instr);
  std::vector<uint8_t> instructionNone(AST::Instruction0& instr0);
  std::vector<uint8_t> instructionR(AST::Instruction1& instr1, const AST::BaseRegister& reg);
  std::vector<uint8_t> instructionRA(AST::Instruction1& instr1, const AST::BaseRegister& reg);
  std::vector<uint8_t> instructionD(AST::Instruction1& instr1, const AST::BaseDRegister& reg);

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
  std::shared_ptr<AST::Root> mAst;
  std::ofstream mOut;

  GBAS::SectionType mCurrSectionType;
  //std::vector<std::pair<std::string, size_t>> mLabels;
};

class AssemblerException : std::exception {
 public:
  AssemblerException(const char* msg) { mMsg = msg; }

  AssemblerException(const std::string& msg) { mMsg = msg; }

  virtual const char* what() const noexcept { return mMsg.c_str(); }

 private:
  std::string mMsg;
};
