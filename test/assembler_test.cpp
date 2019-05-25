
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "assembler.hpp"

/*
 * We need a notion of AST equality. Given two trees, let's walk them both and
 * make sure they're equal.
 *
 * @note We could move this into the individual node definitions if it's useful
 * outside these tests, but otherwise let's isolate the clutter.
 */
static bool isAstEqual(std::shared_ptr<AST::BaseNode> left,
                       std::shared_ptr<AST::BaseNode> right) {
  // Examine only the left node's NodeType. If the right node fails to
  // `dynamic_pointer_cast`, we'll know its type doesn't match.
  switch (left->id()) {
    case AST::NodeType::ROOT:
      {
        auto lRoot = std::dynamic_pointer_cast<AST::Root>(left);
        auto rRoot = std::dynamic_pointer_cast<AST::Root>(right);
        if (lRoot && rRoot) {
          auto lIt = lRoot->begin();
          auto rIt = rRoot->begin();
          while (lIt != lRoot->end() && rIt != rRoot->end()) {
            if (!isAstEqual(*lIt, *rIt)) {
              return false;
            }
          }
          if (lIt != lRoot->end() && rIt != rRoot->end()) {
            // One of the roots has more children
            return false;
          }
        } else {
          return false;
        }
      }
      break;
    case AST::NodeType::INSTRUCTION:
      {
        auto lInstr = std::dynamic_pointer_cast<AST::BaseInstruction>(left);
        auto rInstr = std::dynamic_pointer_cast<AST::BaseInstruction>(right);
        if (lInstr && rInstr) {
          if (lInstr->nOperands() != rInstr->nOperands() ||
              lInstr->type() != rInstr->type()) {
            return false;
          }
        } else {
          return false;
        }
        switch (lInstr->nOperands()) {
          case 0:
            // The only properties we need to check for equality are number of
            // operands and instruction type, which are checked above.
            break;
          case 1:
            {
              auto lInstr1 = std::dynamic_pointer_cast<AST::Instruction1>(lInstr);
              auto rInstr1 = std::dynamic_pointer_cast<AST::Instruction1>(rInstr);
              if (lInstr1 && rInstr1) {
                auto lpOperand = std::shared_ptr<AST::BaseNode>(&lInstr1->operand());
                auto rpOperand = std::shared_ptr<AST::BaseNode>(&rInstr1->operand());
                if (!isAstEqual(lpOperand, rpOperand)) {
                  return false;
                }
              } else {
                return false;
              }
            }
            break;
          case 2:
            {
              auto lInstr2 = std::dynamic_pointer_cast<AST::Instruction2>(lInstr);
              auto rInstr2 = std::dynamic_pointer_cast<AST::Instruction2>(rInstr);
              if (lInstr2 && rInstr2) {
                auto lpLoperand = std::shared_ptr<AST::BaseNode>(&lInstr2->loperand());
                auto lpRoperand = std::shared_ptr<AST::BaseNode>(&lInstr2->roperand());
                auto rpLoperand = std::shared_ptr<AST::BaseNode>(&rInstr2->loperand());
                auto rpRoperand = std::shared_ptr<AST::BaseNode>(&rInstr2->roperand());
                if (!isAstEqual(lpLoperand, rpLoperand) ||
                    !isAstEqual(lpRoperand, rpRoperand)) {
                  return false;
                }
              } else {
                return false;
              }
            }
            break;
          default:
            return false;
        }
      }
      break;
    case AST::NodeType::REGISTER:
      {
        auto lreg = std::dynamic_pointer_cast<AST::BaseRegister>(left);
        auto rreg = std::dynamic_pointer_cast<AST::BaseRegister>(right);
        if (lreg && rreg) {
          if (lreg->reg() != rreg->reg()) {
            return false;
          }
        } else {
          return false;
        }
      }
      break;
    case AST::NodeType::DREGISTER:
      {
        auto lreg = std::dynamic_pointer_cast<AST::BaseDRegister>(left);
        auto rreg = std::dynamic_pointer_cast<AST::BaseDRegister>(right);
        if (lreg && rreg) {
          if (lreg->reg() != rreg->reg()) {
            return false;
          }
        } else {
          return false;
        }
      }
      break;
    case AST::NodeType::LABEL:
      {
        auto llabel = std::dynamic_pointer_cast<AST::Label>(left);
        auto rlabel = std::dynamic_pointer_cast<AST::Label>(right);
        if (llabel && rlabel) {
          if (llabel->name() != rlabel->name()) {
            return false;
          }
        } else {
          return false;
        }
      }
      break;
    case AST::NodeType::NUMBER:
      {
        auto lnum = std::dynamic_pointer_cast<AST::Number>(left);
        auto rnum = std::dynamic_pointer_cast<AST::Number>(right);
        if (lnum && rnum) {
          if (lnum->value() != rnum->value()) {
            return false;
          }
        } else {
          return false;
        }
      }
      break;
    case AST::NodeType::BINARY_OP:
      {
        auto lop = std::dynamic_pointer_cast<AST::BaseBinaryOp>(left);
        auto rop = std::dynamic_pointer_cast<AST::BaseBinaryOp>(right);
        if (lop && rop) {
          if (lop->opType() != rop->opType()) {
            return false;
          } else {
            auto lpLeft = std::shared_ptr<AST::BaseNode>(&lop->left());
            auto lpRight = std::shared_ptr<AST::BaseNode>(&lop->right());
            auto rpLeft = std::shared_ptr<AST::BaseNode>(&rop->left());
            auto rpRight = std::shared_ptr<AST::BaseNode>(&rop->right());
            if (!isAstEqual(lpLeft, rpLeft) ||
                !isAstEqual(lpRight, rpRight)) {
              return false;
            }
          }
        } else {
          return false;
        }
      }
      break;
    default:
      // Rather than raise an exception, we'll just say the trees aren't equal.
      return false;
  }
  return true;
}


BOOST_AUTO_TEST_SUITE(assembler_evaluation);

/*
 * Let's test some basic cases, i.e. things that should evaluate to the same
 * expression.
 */
BOOST_AUTO_TEST_CASE(assembler_test_evaluate_basic) {

}

BOOST_AUTO_TEST_SUITE_END();
