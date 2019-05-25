

#include "parser.hpp"

/**
 * The assembler takes an AST as input and outputs an object file. It should
 * evaluate any constant expressions in the AST (trivially optimize).
 */
class Assembler {
  std::shared_ptr<AST::BaseNode> evaluate(std::shared_ptr<AST::BaseNode>);
  std::shared_ptr<AST::BaseInstruction> evaluateInstruction(std::shared_ptr<AST::BaseInstruction> node);
  std::shared_ptr<AST::BaseNode> evaluateBinaryOp(std::shared_ptr<AST::BaseBinaryOp> node);
  std::shared_ptr<AST::BaseNode> evaluateUnaryOp(std::shared_ptr<AST::BaseUnaryOp> node);
};

class AssemblerException : std::exception {
  public:
    AssemblerException(const char* msg) {
      mMsg = msg;
    }

    AssemblerException(const std::string& msg) {
      mMsg = msg;
    }

    virtual const char* what() const noexcept {
      return mMsg.c_str();
    }

  private:
    std::string mMsg;
};
