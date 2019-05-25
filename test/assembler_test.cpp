
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
                if (!isAstEqual(lInstr1->operand(), rInstr1->operand())) {
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
                if (!isAstEqual(lInstr2->left(), rInstr2->left()) ||
                    !isAstEqual(lInstr2->right(), rInstr2->right())) {
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
            if (!isAstEqual(lop->left(), rop->left()) ||
                !isAstEqual(lop->right(), rop->right())) {
              return false;
            }
          }
        } else {
          return false;
        }
      }
      break;
    case AST::NodeType::UNARY_OP:
      {
        auto lop = std::dynamic_pointer_cast<AST::BaseUnaryOp>(left);
        auto rop = std::dynamic_pointer_cast<AST::BaseUnaryOp>(right);
        if (lop && rop) {
          if (lop->opType() != rop->opType()) {
            return false;
          } else {
            if (!isAstEqual(lop->operand(), rop->operand())) {
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

  { // empty root evaluates to empty root
    auto lroot = std::make_shared<AST::Root>();
    auto rroot = Assembler::evaluate(lroot);
    BOOST_CHECK(isAstEqual(lroot, rroot));
  }

  { // number evaluates to same number
    auto lnum = std::make_shared<AST::Number>(42);
    auto rnum = Assembler::evaluate(lnum);
    BOOST_CHECK(isAstEqual(lnum, rnum));
  }

  { // label evaluates to same label
    auto llabel = std::make_shared<AST::Label>("asdf_1234");
    auto rlabel = Assembler::evaluate(llabel);
    BOOST_CHECK(isAstEqual(llabel, rlabel));
  }

  { // register evaluates to same register
    auto lreg = std::make_shared<AST::Register<'e'>>();
    auto rreg = Assembler::evaluate(lreg);
    BOOST_CHECK(isAstEqual(lreg, rreg));
  }

  { // dregister evaluates to same dregister
    auto lreg = std::make_shared<AST::DRegister<'d', 'e'>>();
    auto rreg = Assembler::evaluate(lreg);
    BOOST_CHECK(isAstEqual(lreg, rreg));
  }

  { // invalid node throws exception
    auto lnode = std::make_shared<AST::Node<AST::NodeType::INVALID>>();
    BOOST_CHECK_THROW(Assembler::evaluate(lnode), AssemblerException);
  }

}

BOOST_AUTO_TEST_CASE(assembler_test_evaluateUnaryOp) {

  { // unary op on register cannot be evaluated
    auto lrand = std::make_shared<AST::Register<'a'>>();
    auto lnode = std::make_shared<AST::UnaryOp<AST::UnaryOpType::NEG>>(lrand);
    auto rnode = Assembler::evaluateUnaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // unary op on label cannot be evaluated (... right?) TODO
    auto lrand = std::make_shared<AST::Label>("asdf");
    auto lnode = std::make_shared<AST::UnaryOp<AST::UnaryOpType::NEG>>(lrand);
    auto rnode = Assembler::evaluateUnaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // unary op on number CAN be evaluated
    auto lrand = std::make_shared<AST::Number>(42);
    auto lnode = std::make_shared<AST::UnaryOp<AST::UnaryOpType::NEG>>(lrand);
    auto rnode = Assembler::evaluateUnaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum);
    BOOST_CHECK(static_cast<int8_t>(rnum->value()) == -42);
  }

  { // invalid unary op raises exception
    auto lrand = std::make_shared<AST::Number>(42);
    auto lnode = std::make_shared<AST::UnaryOp<AST::UnaryOpType::INVALID>>(lrand);
    BOOST_CHECK_THROW(Assembler::evaluateUnaryOp(lnode), AssemblerException);
  }

}

BOOST_AUTO_TEST_CASE(assembler_test_evaluateBinaryOp) {

  { // binary op on register cannot be evaluated
    auto lleft = std::make_shared<AST::Register<'a'>>();
    auto lright = std::make_shared<AST::Register<'b'>>();
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::MULT>>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on label cannot be evaluated TODO
    auto lleft = std::make_shared<AST::Label>("asdf1");
    auto lright = std::make_shared<AST::Label>("asdf2");
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::MULT>>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on label cannot be evaluated TODO
    auto lleft = std::make_shared<AST::Number>(42);
    auto lright = std::make_shared<AST::Label>("asdf2");
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::ADD>>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on numbers CAN be evaluated
    auto lleft = std::make_shared<AST::Number>(21);
    auto lright = std::make_shared<AST::Number>(2);
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::MULT>>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

  { // binary op on numbers CAN be evaluated
    auto lleft = std::make_shared<AST::Number>(40);
    auto lright = std::make_shared<AST::Number>(2);
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::ADD>>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

  { // binary op on numbers CAN be evaluated
    auto lleft = std::make_shared<AST::Number>(50);
    auto lright = std::make_shared<AST::Number>(8);
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::SUB>>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

  { // binary op on numbers CAN be evaluated
    auto lright = std::make_shared<AST::Number>(4);
    auto llleft = std::make_shared<AST::Number>(92);
    auto llright = std::make_shared<AST::Number>(2);
    auto llnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::DIV>>(llleft, llright);
    auto lnode = std::make_shared<AST::BinaryOp<AST::BinaryOpType::SUB>>(llnode, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

}


BOOST_AUTO_TEST_SUITE_END();
