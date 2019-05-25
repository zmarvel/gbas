
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
            if (!isAstEqual(*lIt++, *rIt++)) {
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

/*
 * For debugging purposes.
 */
#define INDENT() std::string(level, ' ')
static void printAst(std::shared_ptr<AST::BaseNode> left, int level) {
  switch (left->id()) {
    case AST::NodeType::ROOT:
      {
        auto lRoot = std::dynamic_pointer_cast<AST::Root>(left);
        std::cout << INDENT() << "(Root" << std::endl;
        auto lIt = lRoot->begin();
        while (lIt != lRoot->end()) {
          printAst(*lIt++, level+1);
        }
        std::cout << INDENT() << ")" << std::endl;
      }
      break;
    case AST::NodeType::INSTRUCTION:
      {
        auto lInstr = std::dynamic_pointer_cast<AST::BaseInstruction>(left);
        switch (lInstr->nOperands()) {
          case 0:
            // The only properties we need to check for equality are number of
            // operands and instruction type, which are checked above.
            break;
          case 1:
            {
              std::cout << INDENT() << "(Instruction1" << std::endl;
              auto lInstr1 = std::dynamic_pointer_cast<AST::Instruction1>(lInstr);
              printAst(lInstr1->operand(), level+1);
              std::cout << INDENT() << ")" << std::endl;
            }
            break;
          case 2:
            {
              auto lInstr2 = std::dynamic_pointer_cast<AST::Instruction2>(lInstr);
              std::cout << INDENT() << "(Instruction2" << std::endl;
              printAst(lInstr2->left(), level+1);
              printAst(lInstr2->right(), level+1);
              std::cout << INDENT() << ")" << std::endl;
            }
            break;
          default:
            return;
        }
      }
      break;
    case AST::NodeType::REGISTER:
      {
        auto lreg = std::dynamic_pointer_cast<AST::BaseRegister>(left);
        std::cout << INDENT() << "(Register " << lreg->reg() << ")" << std::endl;
      }
      break;
    case AST::NodeType::DREGISTER:
      {
        auto lreg = std::dynamic_pointer_cast<AST::BaseDRegister>(left);
        std::cout << INDENT() << "(DRegister " << lreg->reg() << ")" << std::endl;
      }
      break;
    case AST::NodeType::LABEL:
      {
        auto llabel = std::dynamic_pointer_cast<AST::Label>(left);
        std::cout << INDENT() << "(Label " << llabel->name() << ")" << std::endl;
      }
      break;
    case AST::NodeType::NUMBER:
      {
        auto lnum = std::dynamic_pointer_cast<AST::Number>(left);
        std::cout << INDENT() << "(Number " << std::to_string(static_cast<uint32_t>(lnum->value())) << ")" << std::endl;
      }
      break;
    case AST::NodeType::BINARY_OP:
      {
        auto lop = std::dynamic_pointer_cast<AST::BaseBinaryOp>(left);
        std::cout << INDENT() << "(BinaryOp ";
          switch (lop->opType()) {
            case AST::BinaryOpType::ADD:
              std::cout << "ADD" << std::endl;
              break;
            case AST::BinaryOpType::SUB:
              std::cout << "SUB" << std::endl;
              break;
            case AST::BinaryOpType::MULT:
              std::cout << "MULT" << std::endl;
              break;
            case AST::BinaryOpType::DIV:
              std::cout << "DIV" << std::endl;
              break;
          }
          printAst(lop->left(), level+1);
          printAst(lop->right(), level+1);
          std::cout << INDENT() << ")" << std::endl;
      }
      break;
    case AST::NodeType::UNARY_OP:
      {
        auto lop = std::dynamic_pointer_cast<AST::BaseUnaryOp>(left);
        std::cout << INDENT() << "(UnaryOp ";
        switch (lop->opType()) {
          case AST::UnaryOpType::NEG:
            std::cout << "NEG" << std::endl;
            break;
          default:
            break;
        }
        printAst(lop->operand(), level+1);
        std::cout << INDENT() << ")";
      }
      break;
    default:
      // Rather than raise an exception, we'll just say the trees aren't equal.
      return;
  }
  return;
}
#undef INDENT




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

  { // root with children that cannot be evaluated evaluates to same tree
    auto lchild = std::make_shared<AST::Number>(42);
    auto lroot = std::make_shared<AST::Root>();
    lroot->add(lchild);
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
    auto lnode = std::make_shared<AST::NegOp>(lrand);
    auto rnode = Assembler::evaluateUnaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // unary op on label cannot be evaluated (... right?) TODO
    auto lrand = std::make_shared<AST::Label>("asdf");
    auto lnode = std::make_shared<AST::NegOp>(lrand);
    auto rnode = Assembler::evaluateUnaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // unary op on number CAN be evaluated
    auto lrand = std::make_shared<AST::Number>(42);
    auto lnode = std::make_shared<AST::NegOp>(lrand);
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


class InvalidBinaryOp : public AST::BaseBinaryOp {
  public:
    InvalidBinaryOp() :
      AST::BaseBinaryOp{std::shared_ptr<BaseNode>{nullptr},
        std::shared_ptr<BaseNode>{nullptr}}
    { }
};

BOOST_AUTO_TEST_CASE(assembler_test_evaluateBinaryOp) {

  { // binary op on register cannot be evaluated
    auto lleft = std::make_shared<AST::Register<'a'>>();
    auto lright = std::make_shared<AST::Register<'b'>>();
    auto lnode = std::make_shared<AST::MultOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on label cannot be evaluated TODO
    auto lleft = std::make_shared<AST::Label>("asdf1");
    auto lright = std::make_shared<AST::Label>("asdf2");
    auto lnode = std::make_shared<AST::MultOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on label cannot be evaluated TODO
    auto lleft = std::make_shared<AST::Number>(42);
    auto lright = std::make_shared<AST::Label>("asdf2");
    auto lnode = std::make_shared<AST::AddOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on label cannot be evaluated TODO
    auto lleft = std::make_shared<AST::Label>("asdf2");
    auto lright = std::make_shared<AST::Number>(42);
    auto lnode = std::make_shared<AST::SubOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on label cannot be evaluated TODO
    auto lleft = std::make_shared<AST::Register<'d'>>();
    auto lright = std::make_shared<AST::Number>(42);
    auto lnode = std::make_shared<AST::DivOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(isAstEqual(lnode, rnode));
  }

  { // binary op on numbers CAN be evaluated
    auto lleft = std::make_shared<AST::Number>(21);
    auto lright = std::make_shared<AST::Number>(2);
    auto lnode = std::make_shared<AST::MultOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

  { // binary op on numbers CAN be evaluated
    auto lleft = std::make_shared<AST::Number>(40);
    auto lright = std::make_shared<AST::Number>(2);
    auto lnode = std::make_shared<AST::AddOp>(lleft, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

  { // binary op on numbers CAN be evaluated
    auto lleft = std::make_shared<AST::Number>(50);
    auto lright = std::make_shared<AST::Number>(8);
    auto lnode = std::make_shared<AST::SubOp>(lleft, lright);
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
    auto llnode = std::make_shared<AST::DivOp>(llleft, llright);
    auto lnode = std::make_shared<AST::SubOp>(llnode, lright);
    auto rnode = Assembler::evaluateBinaryOp(lnode);
    BOOST_CHECK(!isAstEqual(lnode, rnode));
    BOOST_CHECK(rnode->id() == AST::NodeType::NUMBER);
    auto rnum = std::dynamic_pointer_cast<AST::Number>(rnode);
    BOOST_CHECK(rnum->value() == 42);
  }

  { // invalid binary op should throw
    auto lnode = std::make_shared<InvalidBinaryOp>();
    BOOST_CHECK_THROW(Assembler::evaluateBinaryOp(lnode), AssemblerException);
  }


}


class InvalidInstruction : public AST::Instruction<3> {
  public:
    InvalidInstruction() :
      AST::Instruction<3>{AST::InstructionType::INVALID}
    { }
};

BOOST_AUTO_TEST_CASE(assembler_test_evaluateInstruction) {

  { // instruction with no operands should not change
    auto linstr = std::make_shared<AST::Instruction0>(AST::InstructionType::NOP);
    auto rinstr = Assembler::evaluateInstruction(linstr);
    BOOST_CHECK(isAstEqual(linstr, rinstr));
  }

  { // instruction with one register operand should not change
    auto lrand = std::make_shared<AST::Register<'b'>>();
    auto linstr = std::make_shared<AST::Instruction1>(AST::InstructionType::INC,
                                                      lrand);
    auto rinstr = Assembler::evaluateInstruction(linstr);
    BOOST_CHECK(isAstEqual(linstr, rinstr));
  }

  { // instruction with one register and one label operand should not change
    auto lleft = std::make_shared<AST::Register<'b'>>();
    auto lright = std::make_shared<AST::Label>("asdf");
    auto linstr = std::make_shared<AST::Instruction2>(AST::InstructionType::ADD,
                                                      lleft, lright);
    auto rinstr = Assembler::evaluateInstruction(linstr);
    BOOST_CHECK(isAstEqual(linstr, rinstr));
  }

  { // invalid instruction should throw
    auto linstr = std::make_shared<InvalidInstruction>();
    BOOST_CHECK_THROW(Assembler::evaluateInstruction(linstr), AssemblerException);
  }

  { // instruction with one unary operation as operand should get simplified
    auto lrand = std::make_shared<AST::Number>(static_cast<uint8_t>(-42));
    auto lop = std::make_shared<AST::NegOp>(lrand);
    auto linstr = std::make_shared<AST::Instruction1>(AST::InstructionType::DEC,
                                                      lop);
    auto rinstr = Assembler::evaluateInstruction(linstr);
    BOOST_CHECK(!isAstEqual(linstr, rinstr));
    auto rinstr1 = std::dynamic_pointer_cast<AST::Instruction1>(rinstr);
    BOOST_CHECK(rinstr1->operand()->id() == AST::NodeType::NUMBER);
    auto rrand = std::dynamic_pointer_cast<AST::Number>(rinstr1->operand());
    BOOST_CHECK(rrand->value() == 42);
  }

  { // instruction with one binary operation as operand should get simplified
    auto lleft = std::make_shared<AST::Number>(21);
    auto lright = std::make_shared<AST::Number>(2);
    auto lrand = std::make_shared<AST::MultOp>(lleft, lright);
    auto linstr = std::make_shared<AST::Instruction1>(AST::InstructionType::INC,
                                                      lrand);
    auto rinstr = Assembler::evaluateInstruction(linstr);
    BOOST_CHECK(!isAstEqual(linstr, rinstr));
    auto rinstr1 = std::dynamic_pointer_cast<AST::Instruction1>(rinstr);
    BOOST_CHECK(rinstr1->operand()->id() == AST::NodeType::NUMBER);
    auto rrand = std::dynamic_pointer_cast<AST::Number>(rinstr1->operand());
    BOOST_CHECK(rrand->value() == 42);
  }

  { // instruction with one unary operation and one binary operation as
    // operands should get simplified
    auto l1rand = std::make_shared<AST::Number>(static_cast<uint8_t>(-42));
    auto l1op = std::make_shared<AST::NegOp>(l1rand);
    auto l2left = std::make_shared<AST::Number>(84);
    auto l2right = std::make_shared<AST::Number>(2);
    auto l2op = std::make_shared<AST::DivOp>(l2left, l2right);
    auto linstr = std::make_shared<AST::Instruction2>(AST::InstructionType::ADD,
                                                      l1op, l2op);
    auto rinstr = Assembler::evaluateInstruction(linstr);
    BOOST_CHECK(!isAstEqual(linstr, rinstr));
    auto rinstr2 = std::dynamic_pointer_cast<AST::Instruction2>(rinstr);
    BOOST_CHECK(rinstr2->left()->id() == AST::NodeType::NUMBER);
    BOOST_CHECK(rinstr2->right()->id() == AST::NodeType::NUMBER);
    auto r1num = std::dynamic_pointer_cast<AST::Number>(rinstr2->left());
    BOOST_CHECK(r1num->value() == 42);
    auto r2num = std::dynamic_pointer_cast<AST::Number>(rinstr2->right());
    BOOST_CHECK(r2num->value() == 42);
  }

}

BOOST_AUTO_TEST_CASE(assembler_test_evaluate) {

  { // Now let's evaluate a more interesting "program" that should not get
    // modified during evaluation
    auto instr0 = std::make_shared<AST::Instruction0>(AST::InstructionType::NOP);
    auto instr1 = std::make_shared<AST::Instruction1>(AST::InstructionType::JP,
                                                      std::make_shared<AST::Label>("asdf"));
    auto instr2 = std::make_shared<AST::Instruction2>(AST::InstructionType::LD,
                                                      std::make_shared<AST::DRegister<'b', 'c'>>(),
                                                      std::make_shared<AST::Number>(0x10));
    
    auto instrs = std::vector<std::shared_ptr<AST::BaseNode>>{
      instr0, instr1, instr2
    };
    auto lroot = std::make_shared<AST::Root>(instrs);
    auto rroot = Assembler::evaluate(lroot);
    BOOST_CHECK(isAstEqual(lroot, rroot));
  }

  { // Now a "program" that SHOULD get modified during evaluation
    auto linstr0 = std::make_shared<AST::Instruction0>(AST::InstructionType::NOP);
    auto linstr1rand = std::make_shared<AST::Number>(static_cast<uint8_t>(-42));
    auto linstr1op = std::make_shared<AST::NegOp>(linstr1rand);
    auto linstr1 = std::make_shared<AST::Instruction1>(AST::InstructionType::JP,
                                                      linstr1op);
    auto linstr2op1left = std::make_shared<AST::Number>(static_cast<uint8_t>(-42));
    auto linstr2op1right = std::make_shared<AST::Number>(84);
    auto linstr2op1 = std::make_shared<AST::AddOp>(linstr2op1left, linstr2op1right);
    auto linstr2op2left = std::make_shared<AST::Number>(84);
    auto linstr2op2right = std::make_shared<AST::Number>(static_cast<uint8_t>(2));
    auto linstr2op2 = std::make_shared<AST::NegOp>(std::make_shared<AST::NegOp>(std::make_shared<AST::DivOp>(linstr2op2left, linstr2op2right)));
    auto linstr2 = std::make_shared<AST::Instruction2>(AST::InstructionType::LD,
                                                      linstr2op1, linstr2op2);
    
    auto linstrs = std::vector<std::shared_ptr<AST::BaseNode>>{
      linstr0, linstr1, linstr2
    };
    auto lroot = std::make_shared<AST::Root>(linstrs);
    auto rroot = Assembler::evaluate(lroot);
    BOOST_CHECK(!isAstEqual(lroot, rroot));

    // Let's just build the expected tree and make sure it's equal
    auto rinstr0 = std::make_shared<AST::Instruction0>(AST::InstructionType::NOP);
    auto rinstr1 = std::make_shared<AST::Instruction1>(AST::InstructionType::JP,
                                                      std::make_shared<AST::Number>(42));
    auto rinstr2 = std::make_shared<AST::Instruction2>(AST::InstructionType::LD,
                                                      std::make_shared<AST::Number>(42),
                                                      std::make_shared<AST::Number>(42));
    
    auto rinstrs = std::vector<std::shared_ptr<AST::BaseNode>>{
      rinstr0, rinstr1, rinstr2
    };
    auto rrootExpected = std::make_shared<AST::Root>(rinstrs);
    BOOST_CHECK(isAstEqual(rroot, rrootExpected));
  }

}

BOOST_AUTO_TEST_SUITE_END();
