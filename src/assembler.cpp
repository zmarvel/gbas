
#include <memory>

#include "assembler.hpp"

using namespace AST;

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

std::shared_ptr<BaseNode> Assembler::evaluateBinaryOp(
    std::shared_ptr<BaseBinaryOp> node) {
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
