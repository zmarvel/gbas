
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "char_utils.hpp"

BOOST_AUTO_TEST_SUITE(char_utils_test);

using namespace GBAS;

BOOST_AUTO_TEST_CASE(char_utils_test_isNumber) {
  // We can assume that there are no trailing or leading spaces
  BOOST_CHECK_EQUAL(isNumber(""), false);
  BOOST_CHECK_EQUAL(isNumber("1"), true);
  BOOST_CHECK_EQUAL(isNumber("42"), true);
  BOOST_CHECK_EQUAL(isNumber(" 42"), false);
  BOOST_CHECK_EQUAL(isNumber("42 "), false);
  BOOST_CHECK_EQUAL(isNumber("a42"), false);
  BOOST_CHECK_EQUAL(isNumber("42a"), false);
}

BOOST_AUTO_TEST_CASE(char_utils_test_isNumericOp) {
  // We can assume that there are no trailing or leading spaces
  BOOST_CHECK_EQUAL(isNumericOp(' '), false);
  BOOST_CHECK_EQUAL(isNumericOp('+'), true);
  BOOST_CHECK_EQUAL(isNumericOp('-'), true);
  BOOST_CHECK_EQUAL(isNumericOp('*'), true);
  BOOST_CHECK_EQUAL(isNumericOp('/'), true);
  BOOST_CHECK_EQUAL(isNumericOp('a'), false);
  BOOST_CHECK_EQUAL(isNumericOp('A'), false);
  BOOST_CHECK_EQUAL(isNumericOp('0'), false);
  BOOST_CHECK_EQUAL(isNumericOp('\\'), false);
  BOOST_CHECK_EQUAL(isNumericOp('9'), false);
}

BOOST_AUTO_TEST_SUITE_END();
