
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "parser.h"

BOOST_AUTO_TEST_SUITE(parser_token_predicates);

BOOST_AUTO_TEST_CASE(parser_test_isNumber) {
  // We can assume that there are no trailing or leading spaces
  BOOST_CHECK_EQUAL(Parser::isNumber(""), false);
  BOOST_CHECK_EQUAL(Parser::isNumber("1"), true);
  BOOST_CHECK_EQUAL(Parser::isNumber("42"), true);
  BOOST_CHECK_EQUAL(Parser::isNumber(" 42"), false);
  BOOST_CHECK_EQUAL(Parser::isNumber("42 "), false);
  BOOST_CHECK_EQUAL(Parser::isNumber("a42"), false);
  BOOST_CHECK_EQUAL(Parser::isNumber("42a"), false);
}

BOOST_AUTO_TEST_CASE(parser_test_isNumericOp) {
  // We can assume that there are no trailing or leading spaces
  BOOST_CHECK_EQUAL(Parser::isNumericOp(""), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("+"), true);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("-"), true);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("*"), true);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("/"), true);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("++"), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("--"), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("**"), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("//"), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("1+"), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp(" +"), false);
  BOOST_CHECK_EQUAL(Parser::isNumericOp("+ "), false);
}

BOOST_AUTO_TEST_CASE(parser_test_isInstruction) {
  BOOST_CHECK_EQUAL(Parser::isInstruction(""), false);
  BOOST_CHECK_EQUAL(Parser::isInstruction("42"), false);
  BOOST_CHECK_EQUAL(Parser::isInstruction("+"), false);
  BOOST_CHECK_EQUAL(Parser::isInstruction("label20"), false);

  BOOST_CHECK_EQUAL(Parser::isInstruction("add"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("adc"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("inc"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("sub"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("sbc"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("and"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("xor"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("or"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("cp"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("dec"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rlc"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rlca"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rl"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rla"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rrc"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rrca"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rr"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rra"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("daa"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("scf"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("cpl"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("ccf"), true);

  BOOST_CHECK_EQUAL(Parser::isInstruction("ld"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("ldi"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("ldd"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("push"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("pop"), true);

  BOOST_CHECK_EQUAL(Parser::isInstruction("jr"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("ret"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("jp"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("call"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("rst"), true);

  BOOST_CHECK_EQUAL(Parser::isInstruction("nop"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("stop"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("halt"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("di"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("ei"), true);

  BOOST_CHECK_EQUAL(Parser::isInstruction("sla"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("sra"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("swap"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("srl"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("bit"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("res"), true);
  BOOST_CHECK_EQUAL(Parser::isInstruction("set"), true);
}

BOOST_AUTO_TEST_CASE(parser_test_isLabel) {
  BOOST_CHECK_EQUAL(Parser::isLabel(""), false);
  BOOST_CHECK_EQUAL(Parser::isLabel("42"), false);
  BOOST_CHECK_EQUAL(Parser::isLabel("-"), false);
  BOOST_CHECK_EQUAL(Parser::isLabel(" "), false);
  BOOST_CHECK_EQUAL(Parser::isLabel(":"), false);
  // TODO: Are instructions acceptable labels?
  BOOST_CHECK_EQUAL(Parser::isLabel("add"), false);

  BOOST_CHECK_EQUAL(Parser::isLabel("lbl"), true);
  BOOST_CHECK_EQUAL(Parser::isLabel("lbl1"), true);
  BOOST_CHECK_EQUAL(Parser::isLabel("lbl_1"), true);

  BOOST_CHECK_EQUAL(Parser::isLabel("lbl-1"), false);
  BOOST_CHECK_EQUAL(Parser::isLabel("1lbl"), false);
}

BOOST_AUTO_TEST_CASE(parser_test_isRegister) {
  BOOST_CHECK_EQUAL(Parser::isRegister(""), false);
  BOOST_CHECK_EQUAL(Parser::isRegister("af"), false);
  BOOST_CHECK_EQUAL(Parser::isRegister("a"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister("b"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister("c"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister("d"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister("f"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister("h"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister("l"), true);
  BOOST_CHECK_EQUAL(Parser::isRegister(" l"), false);
}

BOOST_AUTO_TEST_CASE(parser_test_isDRegister) {
  BOOST_CHECK_EQUAL(Parser::isDRegister(""), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("a"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("b"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("c"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("d"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("f"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("h"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("l"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister(" l"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister(" af"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("aa"), false);
  BOOST_CHECK_EQUAL(Parser::isDRegister("af"), true);
  BOOST_CHECK_EQUAL(Parser::isDRegister("bc"), true);
  BOOST_CHECK_EQUAL(Parser::isDRegister("de"), true);
  BOOST_CHECK_EQUAL(Parser::isDRegister("hl"), true);
  BOOST_CHECK_EQUAL(Parser::isDRegister("sp"), true);
  BOOST_CHECK_EQUAL(Parser::isDRegister("pc"), true);
}

BOOST_AUTO_TEST_SUITE_END();



BOOST_AUTO_TEST_SUITE(parser_parse_tokens);

BOOST_AUTO_TEST_CASE(parser_test_parseRegister) {
  Parser parser{};

  BOOST_CHECK_THROW(parser.parseRegister(""), ParserException);
  BOOST_CHECK_THROW(parser.parseRegister("af"), ParserException);
  {
    auto node = parser.parseRegister("a");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto reg = std::static_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), 'a');
  }
  {
    auto node = parser.parseRegister("f");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto reg = std::static_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), 'f');
  }
  {
    auto node = parser.parseRegister("b");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto reg = std::static_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), 'b');
  }
  {
    auto node = parser.parseRegister("c");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto reg = std::static_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), 'c');
  }
  {
    auto node = parser.parseRegister("d");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto reg = std::static_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), 'd');
  }
  {
    auto node = parser.parseRegister("e");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto reg = std::static_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), 'e');
  }
}

BOOST_AUTO_TEST_CASE(parser_test_parseDRegister) {
  Parser parser{};

  BOOST_CHECK_THROW(parser.parseDRegister(""), ParserException);
  BOOST_CHECK_THROW(parser.parseDRegister("a"), ParserException);
  {
    auto node = parser.parseDRegister("af");
    BOOST_CHECK(node->id() == AST::NodeType::DREGISTER);
    auto reg = std::static_pointer_cast<AST::DRegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), "af");
  }
  {
    auto node = parser.parseDRegister("bc");
    BOOST_CHECK(node->id() == AST::NodeType::DREGISTER);
    auto reg = std::static_pointer_cast<AST::DRegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), "bc");
  }
  {
    auto node = parser.parseDRegister("de");
    BOOST_CHECK(node->id() == AST::NodeType::DREGISTER);
    auto reg = std::static_pointer_cast<AST::DRegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), "de");
  }
  {
    auto node = parser.parseDRegister("hl");
    BOOST_CHECK(node->id() == AST::NodeType::DREGISTER);
    auto reg = std::static_pointer_cast<AST::DRegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), "hl");
  }
  {
    auto node = parser.parseDRegister("sp");
    BOOST_CHECK(node->id() == AST::NodeType::DREGISTER);
    auto reg = std::static_pointer_cast<AST::DRegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), "sp");
  }
  {
    auto node = parser.parseDRegister("pc");
    BOOST_CHECK(node->id() == AST::NodeType::DREGISTER);
    auto reg = std::static_pointer_cast<AST::DRegisterBase>(node);
    BOOST_CHECK_EQUAL(reg->reg(), "pc");
  }
}

BOOST_AUTO_TEST_CASE(parser_test_parseNumber) {
  Parser parser{};

  {
    auto node = parser.parseNumber("123");
    BOOST_CHECK(node->id() == AST::NodeType::NUMBER);
    auto num = std::static_pointer_cast<AST::Number>(node);
    BOOST_CHECK_EQUAL(num->value(), 123);
  }
  {
    auto node = parser.parseNumber("-123");
    BOOST_CHECK(node->id() == AST::NodeType::NUMBER);
    auto num = std::static_pointer_cast<AST::Number>(node);
    BOOST_CHECK_EQUAL(num->value(), static_cast<uint8_t>(-123));
  }
  {
    auto node = parser.parseNumber("0");
    BOOST_CHECK(node->id() == AST::NodeType::NUMBER);
    auto num = std::static_pointer_cast<AST::Number>(node);
    BOOST_CHECK_EQUAL(num->value(), 0);
  }
  {
    auto node = parser.parseNumber("notnumber");
    BOOST_CHECK(node->id() == AST::NodeType::NUMBER);
    auto num = std::static_pointer_cast<AST::Number>(node);
    BOOST_CHECK_EQUAL(num->value(), 0);
  }
  {
    auto node = parser.parseNumber("255");
    BOOST_CHECK(node->id() == AST::NodeType::NUMBER);
    auto num = std::static_pointer_cast<AST::Number>(node);
    BOOST_CHECK_EQUAL(num->value(), 255);
  }
}

BOOST_AUTO_TEST_CASE(parser_test_parseNumericOp) {
  Parser parser{};

  { // Both children are numbers
    auto lnode = std::make_shared<AST::Number>(123);
    auto rnode = std::make_shared<AST::Number>(112);
    auto node = parser.parseNumericOp(lnode, "+", rnode);
    BOOST_CHECK(node->id() == AST::NodeType::NUMERIC_OP);
    auto op = std::dynamic_pointer_cast<AST::BaseNumericOp>(node);
    BOOST_CHECK(op);
    BOOST_CHECK(op->opType() == AST::BinaryOp::ADD);
    auto addOp = std::dynamic_pointer_cast<AST::NumericOp<AST::BinaryOp::ADD>>(node);
    BOOST_CHECK(addOp);
    BOOST_CHECK(addOp->left().id() == AST::NodeType::NUMBER);
    BOOST_CHECK(addOp->right().id() == AST::NodeType::NUMBER);
  }

  { // Left child is number, right child is another NumericOp
    auto lnode = std::make_shared<AST::Number>(123);
    auto rnode = std::make_shared<AST::NumericOp<AST::BinaryOp::SUB>>(std::make_shared<AST::Number>(200),
                                                                      std::make_shared<AST::Number>(20));
    auto node = parser.parseNumericOp(lnode, "-", rnode);
    BOOST_CHECK(node->id() == AST::NodeType::NUMERIC_OP);
    auto op = std::dynamic_pointer_cast<AST::BaseNumericOp>(node);
    BOOST_CHECK(op);
    BOOST_CHECK(op->opType() == AST::BinaryOp::SUB);
    auto subOp = std::dynamic_pointer_cast<AST::NumericOp<AST::BinaryOp::SUB>>(node);
    BOOST_CHECK(subOp);
    BOOST_CHECK(subOp->left().id() == AST::NodeType::NUMBER);
    BOOST_CHECK(subOp->right().id() == AST::NodeType::NUMERIC_OP);
  }

  { // Left child is a NumericOp, right child is a number
    auto lnode = std::make_shared<AST::NumericOp<AST::BinaryOp::SUB>>(std::make_shared<AST::Number>(200),
                                                                      std::make_shared<AST::Number>(20));
    auto rnode = std::make_shared<AST::Number>(123);
    auto node = parser.parseNumericOp(lnode, "*", rnode);
    BOOST_CHECK(node->id() == AST::NodeType::NUMERIC_OP);
    auto op = std::dynamic_pointer_cast<AST::BaseNumericOp>(node);
    BOOST_CHECK(op);
    BOOST_CHECK(op->opType() == AST::BinaryOp::MULT);
    auto multOp = std::dynamic_pointer_cast<AST::NumericOp<AST::BinaryOp::MULT>>(node);
    BOOST_CHECK(multOp);
    BOOST_CHECK(multOp->left().id() == AST::NodeType::NUMERIC_OP);
    BOOST_CHECK(multOp->right().id() == AST::NodeType::NUMBER);
  }

  { // Both children are NumericOps
    auto lnode = std::make_shared<AST::NumericOp<AST::BinaryOp::SUB>>(std::make_shared<AST::Number>(200),
                                                                      std::make_shared<AST::Number>(20));
    auto rnode = std::make_shared<AST::NumericOp<AST::BinaryOp::SUB>>(std::make_shared<AST::Number>(200),
                                                                      std::make_shared<AST::Number>(20));
    auto node = parser.parseNumericOp(lnode, "/", rnode);
    BOOST_CHECK(node->id() == AST::NodeType::NUMERIC_OP);
    auto op = std::dynamic_pointer_cast<AST::BaseNumericOp>(node);
    BOOST_CHECK(op);
    BOOST_CHECK(op->opType() == AST::BinaryOp::DIV);
    auto divOp = std::dynamic_pointer_cast<AST::NumericOp<AST::BinaryOp::DIV>>(node);
    BOOST_CHECK(divOp);
    BOOST_CHECK(divOp->left().id() == AST::NodeType::NUMERIC_OP);
    BOOST_CHECK(divOp->right().id() == AST::NodeType::NUMERIC_OP);
  }
}

BOOST_AUTO_TEST_CASE(parser_test_parseInstruction) {
  Parser parser{};

  // Invalid instructions should throw
  BOOST_CHECK_THROW(parser.parseInstruction(""), ParserException);
  BOOST_CHECK_THROW(parser.parseInstruction("asdf"), ParserException);
  BOOST_CHECK_THROW(parser.parseInstruction("asdf",
                                            std::make_shared<AST::Register<'a'>>()),
                    ParserException);
  BOOST_CHECK_THROW(parser.parseInstruction("asdf",
                                            std::make_shared<AST::Register<'a'>>(),
                                            std::make_shared<AST::Register<'a'>>()),
                    ParserException);



  // Invalid arg count should throw
  BOOST_CHECK_THROW(parser.parseInstruction("add"), ParserException);
  BOOST_CHECK_THROW(parser.parseInstruction("add", std::make_shared<AST::Register<'a'>>()),
                    ParserException);
  BOOST_CHECK_THROW(parser.parseInstruction("nop",
                                            std::make_shared<AST::Register<'a'>>(),
                                            std::make_shared<AST::Register<'a'>>()),
                    ParserException);

  {
    auto node = parser.parseInstruction("nop");
    BOOST_CHECK(node->id() == AST::NodeType::INSTRUCTION);
    auto inst = std::dynamic_pointer_cast<AST::BaseInstruction>(node);
    BOOST_CHECK(inst);
    BOOST_CHECK(inst->type() == AST::InstructionType::NOP);
    BOOST_CHECK(inst->nOperands() == 0);
  }

  {
    auto node = parser.parseInstruction("inc", std::make_shared<AST::Register<'a'>>());
    BOOST_CHECK(node->id() == AST::NodeType::INSTRUCTION);
    auto inst = std::dynamic_pointer_cast<AST::BaseInstruction>(node);
    BOOST_CHECK(inst);
    BOOST_CHECK(inst->type() == AST::InstructionType::INC);
    BOOST_CHECK(inst->nOperands() == 1);
    auto inst1 = std::dynamic_pointer_cast<AST::Instruction1>(inst);
    BOOST_CHECK(inst1);
    auto& baseOperand = inst1->operand();
    BOOST_CHECK(baseOperand.id() == AST::NodeType::REGISTER);
    auto operand = dynamic_cast<AST::Register<'a'>&>(baseOperand);
    BOOST_CHECK(operand.reg() == 'a');
  }

  {
    auto node = parser.parseInstruction("add",
                                        std::make_shared<AST::Register<'a'>>(),
                                        std::make_shared<AST::Number>(1));
    BOOST_CHECK(node->id() == AST::NodeType::INSTRUCTION);
    auto inst = std::dynamic_pointer_cast<AST::BaseInstruction>(node);
    BOOST_CHECK(inst);
    BOOST_CHECK(inst->type() == AST::InstructionType::ADD);
    BOOST_CHECK(inst->nOperands() == 2);
    auto inst2 = std::dynamic_pointer_cast<AST::Instruction2>(inst);
    BOOST_CHECK(inst2);
    auto& baseLoperand = inst2->loperand();
    BOOST_CHECK(baseLoperand.id() == AST::NodeType::REGISTER);
    auto loperand = dynamic_cast<AST::Register<'a'>&>(baseLoperand);
    BOOST_CHECK(loperand.reg() == 'a');

    auto& baseRoperand = inst2->roperand();
    BOOST_CHECK(baseRoperand.id() == AST::NodeType::NUMBER);
    auto roperand = dynamic_cast<AST::Number&>(baseRoperand);
    BOOST_CHECK(roperand.value() == 1);
  }

  { // One-argument version of sub
    auto node = parser.parseInstruction("sub", std::make_shared<AST::Number>(1));
    BOOST_CHECK(node->id() == AST::NodeType::INSTRUCTION);
    auto inst = std::dynamic_pointer_cast<AST::BaseInstruction>(node);
    BOOST_CHECK(inst);
    BOOST_CHECK(inst->type() == AST::InstructionType::SUB);
    BOOST_CHECK(inst->nOperands() == 1);
    auto inst1 = std::dynamic_pointer_cast<AST::Instruction1>(inst);
    BOOST_CHECK(inst1);

    auto& baseOperand = inst1->operand();
    BOOST_CHECK(baseOperand.id() == AST::NodeType::NUMBER);
    auto operand = dynamic_cast<AST::Number&>(baseOperand);
    BOOST_CHECK(operand.value() == 1);
  }

  { // Two-argument version of sub
    auto node = parser.parseInstruction("sub", std::make_shared<AST::Number>(1),
                                        std::make_shared<AST::Number>(2));
    BOOST_CHECK(node->id() == AST::NodeType::INSTRUCTION);
    auto inst = std::dynamic_pointer_cast<AST::BaseInstruction>(node);
    BOOST_CHECK(inst);
    BOOST_CHECK(inst->type() == AST::InstructionType::SUB);
    BOOST_CHECK(inst->nOperands() == 2);
    auto inst2 = std::dynamic_pointer_cast<AST::Instruction2>(inst);
    BOOST_CHECK(inst2);

    auto& baseLoperand = inst2->loperand();
    BOOST_CHECK(baseLoperand.id() == AST::NodeType::NUMBER);
    auto loperand = dynamic_cast<AST::Number&>(baseLoperand);
    BOOST_CHECK(loperand.value() == 1);

    auto& baseRoperand = inst2->roperand();
    BOOST_CHECK(baseRoperand.id() == AST::NodeType::NUMBER);
    auto roperand = dynamic_cast<AST::Number&>(baseRoperand);
    BOOST_CHECK(roperand.value() == 2);
  }
}

BOOST_AUTO_TEST_CASE(parser_test_parseLabel) {
  Parser parser{};

  BOOST_CHECK_THROW(parser.parseLabel("1asdf"), ParserException);
  BOOST_CHECK_THROW(parser.parseLabel(""), ParserException);
  BOOST_CHECK_THROW(parser.parseLabel(" "), ParserException);
  BOOST_CHECK_THROW(parser.parseLabel("asdf-1"), ParserException);
  BOOST_CHECK_THROW(parser.parseLabel("a@"), ParserException);
  BOOST_CHECK_THROW(parser.parseLabel("a:"), ParserException);

  {
    auto node = parser.parseLabel("asdf12aa23");
    BOOST_CHECK(node->id() == AST::NodeType::LABEL);
    auto label = std::dynamic_pointer_cast<AST::Label>(node);
    BOOST_CHECK(label);
    BOOST_CHECK(label->name() == "asdf12aa23");
  }

}

BOOST_AUTO_TEST_CASE(parser_test_parseOperand) {
  Parser parser{};

  BOOST_CHECK_THROW(parser.parseOperand(""), ParserException);
  // Instructions are not valid labels
  BOOST_CHECK_THROW(parser.parseOperand("halt"), ParserException);
  // Can't start with an operand
  BOOST_CHECK_THROW(parser.parseOperand("+"), ParserException);

  { // Labels are valid operands
    auto node = parser.parseOperand("asdf12aa23");
    BOOST_CHECK(node->id() == AST::NodeType::LABEL);
    auto label = std::dynamic_pointer_cast<AST::Label>(node);
    BOOST_CHECK(label);
    BOOST_CHECK(label->name() == "asdf12aa23");
  }

  { // Numbers are valid operands
    auto node = parser.parseOperand("123");
    BOOST_CHECK(node->id() == AST::NodeType::NUMBER);
    auto num = std::dynamic_pointer_cast<AST::Number>(node);
    BOOST_CHECK(num);
    BOOST_CHECK(num->value() == 123);
  }

  { // Registers are valid operands
    auto node = parser.parseOperand("d");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
    auto regBase = std::dynamic_pointer_cast<AST::RegisterBase>(node);
    BOOST_CHECK(regBase);
    BOOST_CHECK(regBase->reg() == 'd');
    auto reg = std::dynamic_pointer_cast<AST::Register<'d'>>(node);
    BOOST_CHECK(reg);
  }
}

BOOST_AUTO_TEST_SUITE_END();
