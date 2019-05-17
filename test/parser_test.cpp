
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
  }
  {
    auto node = parser.parseRegister("f");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
  }
  {
    auto node = parser.parseRegister("b");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
  }
  {
    auto node = parser.parseRegister("c");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
  }
  {
    auto node = parser.parseRegister("d");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
  }
  {
    auto node = parser.parseRegister("e");
    BOOST_CHECK(node->id() == AST::NodeType::REGISTER);
  }
}

BOOST_AUTO_TEST_SUITE_END();
