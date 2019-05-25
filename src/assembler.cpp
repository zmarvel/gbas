
#include <memory>

#include "assembler.hpp"


using namespace AST;

std::shared_ptr<BaseNode> Assembler::evaluate(std::shared_ptr<BaseNode> node) {
  switch (node->id()) {
    case NodeType::ROOT:
      {
        auto root = std::dynamic_pointer_cast<Root>(node);
        auto newRoot = std::make_shared<Root>();
        if (root) {
          for (auto it = root->begin(); it != root->end(); it++) {
            newRoot->add(evaluate(*it));
          }
          return newRoot;
        } else {
          throw AssemblerException("Invalid AST Root node");
        }
      }
      break;
    case NodeType::INSTRUCTION:
      {
        auto instr = std::dynamic_pointer_cast<BaseInstruction>(node);
        if (instr) {
          return evaluateInstruction(instr);
        } else {
          throw AssemblerException("Invalid AST Instruction node");
        }
      }
      break;
    case NodeType::REGISTER:
      {
        auto reg = std::dynamic_pointer_cast<BaseRegister>(node);
        if (reg) {
          // Can't evaluate a register.
          return reg;
        } else {
          throw AssemblerException("Invalid AST Register node");
        }
      }
      break;
    case NodeType::DREGISTER:
      {
        auto reg = std::dynamic_pointer_cast<BaseDRegister>(node);
        if (reg) {
          // Can't evaluate a dregister.
          return reg;
        } else {
          throw AssemblerException("Invalid AST DRegister node");
        }
      }
      break;
    case NodeType::LABEL:
      {
        auto label = std::dynamic_pointer_cast<Label>(node);
        if (label) {
          // Can't evaluate a label until link-time.
          return label;
        } else {
          throw AssemblerException("Invalid AST Label node");
        }
      }
      break;
    case NodeType::NUMBER:
      {
        auto num = std::dynamic_pointer_cast<Number>(node);
        if (num) {
          // A number's value is just itself.
          return num;
        } else {
          throw AssemblerException("Invalid AST Number node");
        }
      }
      break;
    case NodeType::BINARY_OP:
      {
        auto op = std::dynamic_pointer_cast<BaseBinaryOp>(node);
        if (op) {
          return evaluateBinaryOp(op);
        } else {
          throw AssemblerException("Invalid AST BinaryOp node");
        }
      }
      break;
    case NodeType::UNARY_OP:
      {
        auto op = std::dynamic_pointer_cast<BaseUnaryOp>(node);
        if (op) {
          return evaluateUnaryOp(op);
        } else {
          throw AssemblerException("Invalid AST UnaryOp node");
        }
      }
      break;
    case NodeType::INVALID:
    default:
      throw AssemblerException("Unrecognized AST node type");
  }
}

std::shared_ptr<BaseInstruction> Assembler::evaluateInstruction(std::shared_ptr<BaseInstruction> node) {
  switch (node->nOperands()) {
    case 0:
      return node;
      break;
    case 1:
      {
        auto instr1 = std::dynamic_pointer_cast<Instruction1>(node);
        if (instr1) {
          auto pOperand = std::shared_ptr<BaseNode>(&(instr1->operand()));
          return std::make_shared<Instruction1>(instr1->type(),
                                                evaluate(pOperand));
        } else {
          throw AssemblerException("Invalid AST Instruction1 node");
        }
      }
      break;
    case 2:
      {
        auto instr2 = std::dynamic_pointer_cast<Instruction2>(node);
        if (instr2) {
          auto pLoperand = std::shared_ptr<BaseNode>(&(instr2->loperand()));
          auto pRoperand = std::shared_ptr<BaseNode>(&(instr2->roperand()));
          return std::make_shared<Instruction2>(instr2->type(),
                                                evaluate(pLoperand),
                                                evaluate(pRoperand));
        } else {
          throw AssemblerException("Invalid AST Instruction2 node");
        }
      }
      break;
    default:
      throw AssemblerException("Invalid number of Instruction operands");
  }
}

std::shared_ptr<BaseNode> Assembler::evaluateBinaryOp(std::shared_ptr<BaseBinaryOp> node) {
  switch (node->opType()) {
    case BinaryOpType::ADD:
      {
        auto op = std::dynamic_pointer_cast<AddOp>(node);
        if (node) {
          auto pLeft = std::shared_ptr<BaseNode>(&op->left());
          auto pRight = std::shared_ptr<BaseNode>(&op->right());
          auto vLeft = evaluate(pLeft);
          auto vRight = evaluate(pRight);
          if (vLeft->id() == NodeType::NUMBER &&
              vRight->id() == NodeType::NUMBER) {
            auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
            auto rnum = std::dynamic_pointer_cast<Number>(vRight);
            if (lnum && rnum) {
              return std::make_shared<Number>(lnum->value() + rnum->value());
            } else {
              throw AssemblerException("Invalid BinaryOp child");
            }
          } else {
            // Can't evaluate anything
            return std::make_shared<AddOp>(vLeft, vRight);
          }
        } else {
          throw AssemblerException("Invalid AST BinaryOp node");
        }
      }
      break;
    case BinaryOpType::SUB:
      {
        auto op = std::dynamic_pointer_cast<SubOp>(node);
        if (node) {
          auto pLeft = std::shared_ptr<BaseNode>(&op->left());
          auto pRight = std::shared_ptr<BaseNode>(&op->right());
          auto vLeft = evaluate(pLeft);
          auto vRight = evaluate(pRight);
          if (vLeft->id() == NodeType::NUMBER &&
              vRight->id() == NodeType::NUMBER) {
            auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
            auto rnum = std::dynamic_pointer_cast<Number>(vRight);
            if (lnum && rnum) {
              return std::make_shared<Number>(lnum->value() - rnum->value());
            } else {
              throw AssemblerException("Invalid BinaryOp child");
            }
          } else {
            // Can't evaluate anything
            return std::make_shared<SubOp>(vLeft, vRight);
          }
        } else {
          throw AssemblerException("Invalid AST BinaryOp node");
        }
      }
      break;
    case BinaryOpType::MULT:
      {
        auto op = std::dynamic_pointer_cast<MultOp>(node);
        if (node) {
          auto pLeft = std::shared_ptr<BaseNode>(&op->left());
          auto pRight = std::shared_ptr<BaseNode>(&op->right());
          auto vLeft = evaluate(pLeft);
          auto vRight = evaluate(pRight);
          if (vLeft->id() == NodeType::NUMBER &&
              vRight->id() == NodeType::NUMBER) {
            auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
            auto rnum = std::dynamic_pointer_cast<Number>(vRight);
            if (lnum && rnum) {
              return std::make_shared<Number>(lnum->value() * rnum->value());
            } else {
              throw AssemblerException("Invalid BinaryOp child");
            }
          } else {
            // Can't evaluate anything
            return std::make_shared<MultOp>(vLeft, vRight);
          }
        } else {
          throw AssemblerException("Invalid AST BinaryOp node");
        }
      }
      break;
    case BinaryOpType::DIV:
      {
        auto op = std::dynamic_pointer_cast<DivOp>(node);
        if (node) {
          auto pLeft = std::shared_ptr<BaseNode>(&op->left());
          auto pRight = std::shared_ptr<BaseNode>(&op->right());
          auto vLeft = evaluate(pLeft);
          auto vRight = evaluate(pRight);
          if (vLeft->id() == NodeType::NUMBER &&
              vRight->id() == NodeType::NUMBER) {
            auto lnum = std::dynamic_pointer_cast<Number>(vLeft);
            auto rnum = std::dynamic_pointer_cast<Number>(vRight);
            if (lnum && rnum) {
              return std::make_shared<Number>(lnum->value() / rnum->value());
            } else {
              throw AssemblerException("Invalid BinaryOp child");
            }
          } else {
            // Can't evaluate anything
            return std::make_shared<DivOp>(vLeft, vRight);
          }
        } else {
          throw AssemblerException("Invalid AST BinaryOp node");
        }
      }
      break;
    default:
      throw AssemblerException("Invalid BinaryOpType");
  }
}

std::shared_ptr<BaseNode> Assembler::evaluateUnaryOp(std::shared_ptr<BaseUnaryOp> node) {
  switch (node->opType()) {
    case UnaryOpType::NEG:
      {
        auto op = std::dynamic_pointer_cast<NegOp>(node);
        if (node) {
          auto pRand = std::shared_ptr<BaseNode>(&op->operand());
          auto vRand = evaluate(pRand);
          if (vRand->id() == NodeType::NUMBER) {
            auto num = std::dynamic_pointer_cast<Number>(vRand);
            if (num) {
              return std::make_shared<Number>(-num->value());
            } else {
              throw AssemblerException("Invalid UnaryOp child");
            }
          } else {
            // Can't evaluate anything
            return std::make_shared<NegOp>(vRand);
          }
        } else {
          throw AssemblerException("Invalid AST UnaryOp node");
        }
      }
      break;
    default:
      throw AssemblerException("Invalid UnaryOpType");
  }
}
