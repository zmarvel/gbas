
#include <fstream>
#include <string>
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "tokenizer.hpp"

BOOST_AUTO_TEST_CASE(tokenizer_test_isReserved) {
  BOOST_TEST(Tokenizer::isReserved("add") == false);
  BOOST_TEST(Tokenizer::isReserved("73") == false);
  BOOST_TEST(Tokenizer::isReserved(".section") == false);
  BOOST_TEST(Tokenizer::isReserved("EOL") == true);
}

BOOST_AUTO_TEST_CASE(tokenizer_test_tokenize0) {
  auto tokenizer = Tokenizer{};
  auto input = "add a, 32\n";
  std::stringstream stream;
  stream << input;
  auto tokens = tokenizer.tokenize(stream);
  BOOST_REQUIRE_EQUAL(tokens.size(), 6);
  BOOST_REQUIRE_EQUAL(tokens.at(0), Token("add"));
  BOOST_REQUIRE_EQUAL(tokens.at(1), Token("a"));
  BOOST_REQUIRE_EQUAL(tokens.at(2), Token(","));
  BOOST_REQUIRE_EQUAL(tokens.at(3), Token("32"));
  BOOST_REQUIRE_EQUAL(tokens.at(4), Token("EOL"));
  BOOST_REQUIRE_EQUAL(tokens.at(5), Token("EOF"));
}

class TokenizerTestFile {
 public:
  explicit TokenizerTestFile(std::string filename, std::string expectedFilename)
      : filename{filename},
        expectedFilename{expectedFilename},
        stream{std::string{"../test/data/"} + filename},
        expectedStream{std::string{"../test/data/"} + expectedFilename} {}

  ~TokenizerTestFile() {
    stream.close();
    expectedStream.close();
  }

  void doTest() {
    if (!stream.is_open()) {
      BOOST_FAIL("Input file does not exist");
    } else if (!expectedStream.is_open()) {
      BOOST_FAIL("Expected output file does not exist");
    }
    auto tokenizer = Tokenizer{};
    auto tokens = tokenizer.tokenize(stream);
    auto token = tokens.begin();
    for (std::string line;
         std::getline(expectedStream, line) && (token != tokens.end());
         line.clear()) {
      BOOST_REQUIRE_EQUAL(line, *token);
      token++;
    }
    BOOST_TEST(expectedStream.eof());
    if (token != tokens.end()) {
      BOOST_FAIL("token != tokens.end()");
    }
  }

  static const int MAX_LINELEN = 256;

 private:
  std::string filename;
  std::string expectedFilename;
  std::ifstream stream;
  std::ifstream expectedStream;
};

BOOST_AUTO_TEST_CASE(tokenizer_test_arith0) {
  TokenizerTestFile testFile{"arith0.asm", "arith0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_arith1) {
  TokenizerTestFile testFile{"arith1.asm", "arith1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_arith2) {
  TokenizerTestFile testFile{"arith2.asm", "arith2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_comment0) {
  TokenizerTestFile testFile{"comment0.asm", "comment0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_comment1) {
  TokenizerTestFile testFile{"comment1.asm", "comment1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_comment2) {
  TokenizerTestFile testFile{"comment2.asm", "comment2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_align0) {
  TokenizerTestFile testFile{"directive_align0.asm", "directive_align0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_align1) {
  TokenizerTestFile testFile{"directive_align1.asm", "directive_align1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ascii0) {
  TokenizerTestFile testFile{"directive_ascii0.asm", "directive_ascii0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ascii1) {
  TokenizerTestFile testFile{"directive_ascii1.asm", "directive_ascii1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ascii2) {
  TokenizerTestFile testFile{"directive_ascii2.asm", "directive_ascii2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ascii3) {
  TokenizerTestFile testFile{"directive_ascii3.asm", "directive_ascii3.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_asciz0) {
  TokenizerTestFile testFile{"directive_asciz0.asm", "directive_asciz0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_asciz1) {
  TokenizerTestFile testFile{"directive_asciz1.asm", "directive_asciz1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_balign0) {
  TokenizerTestFile testFile{"directive_balign0.asm",
                             "directive_balign0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_balign1) {
  TokenizerTestFile testFile{"directive_balign1.asm",
                             "directive_balign1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_balign2) {
  TokenizerTestFile testFile{"directive_balign2.asm",
                             "directive_balign2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_balign3) {
  TokenizerTestFile testFile{"directive_balign3.asm",
                             "directive_balign3.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_data0) {
  TokenizerTestFile testFile{"directive_data0.asm", "directive_data0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_data1) {
  TokenizerTestFile testFile{"directive_data1.asm", "directive_data1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dc0) {
  TokenizerTestFile testFile{"directive_dc0.asm", "directive_dc0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dc1) {
  TokenizerTestFile testFile{"directive_dc1.asm", "directive_dc1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dc2) {
  TokenizerTestFile testFile{"directive_dc2.asm", "directive_dc2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dc3) {
  TokenizerTestFile testFile{"directive_dc3.asm", "directive_dc3.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dc4) {
  TokenizerTestFile testFile{"directive_dc4.asm", "directive_dc4.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dc5) {
  TokenizerTestFile testFile{"directive_dc5.asm", "directive_dc5.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dcb0) {
  TokenizerTestFile testFile{"directive_dcb0.asm", "directive_dcb0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dcb1) {
  TokenizerTestFile testFile{"directive_dcb1.asm", "directive_dcb1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dcb2) {
  TokenizerTestFile testFile{"directive_dcb2.asm", "directive_dcb2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_dcb3) {
  TokenizerTestFile testFile{"directive_dcb3.asm", "directive_dcb3.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_double0) {
  TokenizerTestFile testFile{"directive_double0.asm",
                             "directive_double0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_double1) {
  TokenizerTestFile testFile{"directive_double1.asm",
                             "directive_double1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_double2) {
  TokenizerTestFile testFile{"directive_double2.asm",
                             "directive_double2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ds0) {
  TokenizerTestFile testFile{"directive_ds0.asm", "directive_ds0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ds1) {
  TokenizerTestFile testFile{"directive_ds1.asm", "directive_ds1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ds2) {
  TokenizerTestFile testFile{"directive_ds2.asm", "directive_ds2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ds3) {
  TokenizerTestFile testFile{"directive_ds3.asm", "directive_ds3.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_equ) {
  TokenizerTestFile testFile{"directive_equ.asm", "directive_equ.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_equiv) {
  TokenizerTestFile testFile{"directive_equiv.asm", "directive_equiv.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_eqv) {
  TokenizerTestFile testFile{"directive_eqv.asm", "directive_eqv.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_err) {
  TokenizerTestFile testFile{"directive_err.asm", "directive_err.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_error0) {
  TokenizerTestFile testFile{"directive_error0.asm", "directive_error0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_error1) {
  TokenizerTestFile testFile{"directive_error1.asm", "directive_error1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_fail0) {
  TokenizerTestFile testFile{"directive_fail0.asm", "directive_fail0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_fail1) {
  TokenizerTestFile testFile{"directive_fail1.asm", "directive_fail1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_fill0) {
  TokenizerTestFile testFile{"directive_fill0.asm", "directive_fill0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_fill1) {
  TokenizerTestFile testFile{"directive_fill1.asm", "directive_fill1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_fill2) {
  TokenizerTestFile testFile{"directive_fill2.asm", "directive_fill2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_func0) {
  TokenizerTestFile testFile{"directive_func0.asm", "directive_func0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_func1) {
  TokenizerTestFile testFile{"directive_func1.asm", "directive_func1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_global) {
  TokenizerTestFile testFile{"directive_global.asm", "directive_global.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_globl) {
  TokenizerTestFile testFile{"directive_globl.asm", "directive_globl.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_hidden) {
  TokenizerTestFile testFile{"directive_hidden.asm", "directive_hidden.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_hword0) {
  TokenizerTestFile testFile{"directive_hword0.asm", "directive_hword0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_hword1) {
  TokenizerTestFile testFile{"directive_hword1.asm", "directive_hword1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_if0) {
  TokenizerTestFile testFile{"directive_if0.asm", "directive_if0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_if1) {
  TokenizerTestFile testFile{"directive_if1.asm", "directive_if1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_if2) {
  TokenizerTestFile testFile{"directive_if2.asm", "directive_if2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifb0) {
  TokenizerTestFile testFile{"directive_ifb0.asm", "directive_ifb0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifb1) {
  TokenizerTestFile testFile{"directive_ifb1.asm", "directive_ifb1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifc0) {
  TokenizerTestFile testFile{"directive_ifc0.asm", "directive_ifc0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifc1) {
  TokenizerTestFile testFile{"directive_ifc1.asm", "directive_ifc1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifc2) {
  TokenizerTestFile testFile{"directive_ifc2.asm", "directive_ifc2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifdef0) {
  TokenizerTestFile testFile{"directive_ifdef0.asm", "directive_ifdef0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifdef1) {
  TokenizerTestFile testFile{"directive_ifdef1.asm", "directive_ifdef1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifdef2) {
  TokenizerTestFile testFile{"directive_ifdef2.asm", "directive_ifdef2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifeq0) {
  TokenizerTestFile testFile{"directive_ifeq0.asm", "directive_ifeq0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifeq1) {
  TokenizerTestFile testFile{"directive_ifeq1.asm", "directive_ifeq1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifeq2) {
  TokenizerTestFile testFile{"directive_ifeq2.asm", "directive_ifeq2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifeqs0) {
  TokenizerTestFile testFile{"directive_ifeqs0.asm", "directive_ifeqs0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifeqs1) {
  TokenizerTestFile testFile{"directive_ifeqs1.asm", "directive_ifeqs1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifeqs2) {
  TokenizerTestFile testFile{"directive_ifeqs2.asm", "directive_ifeqs2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifge0) {
  TokenizerTestFile testFile{"directive_ifge0.asm", "directive_ifge0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifge1) {
  TokenizerTestFile testFile{"directive_ifge1.asm", "directive_ifge1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifge2) {
  TokenizerTestFile testFile{"directive_ifge2.asm", "directive_ifge2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifgt0) {
  TokenizerTestFile testFile{"directive_ifgt0.asm", "directive_ifgt0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifgt1) {
  TokenizerTestFile testFile{"directive_ifgt1.asm", "directive_ifgt1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifgt2) {
  TokenizerTestFile testFile{"directive_ifgt2.asm", "directive_ifgt2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifle0) {
  TokenizerTestFile testFile{"directive_ifle0.asm", "directive_ifle0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifle1) {
  TokenizerTestFile testFile{"directive_ifle1.asm", "directive_ifle1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifle2) {
  TokenizerTestFile testFile{"directive_ifle2.asm", "directive_ifle2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_iflt0) {
  TokenizerTestFile testFile{"directive_iflt0.asm", "directive_iflt0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_iflt1) {
  TokenizerTestFile testFile{"directive_iflt1.asm", "directive_iflt1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifnb0) {
  TokenizerTestFile testFile{"directive_ifnb0.asm", "directive_ifnb0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifnb1) {
  TokenizerTestFile testFile{"directive_ifnb1.asm", "directive_ifnb1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifnc0) {
  TokenizerTestFile testFile{"directive_ifnc0.asm", "directive_ifnc0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifnc1) {
  TokenizerTestFile testFile{"directive_ifnc1.asm", "directive_ifnc1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifndef0) {
  TokenizerTestFile testFile{"directive_ifndef0.asm",
                             "directive_ifndef0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifndef1) {
  TokenizerTestFile testFile{"directive_ifndef1.asm",
                             "directive_ifndef1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifne0) {
  TokenizerTestFile testFile{"directive_ifne0.asm", "directive_ifne0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifne1) {
  TokenizerTestFile testFile{"directive_ifne1.asm", "directive_ifne1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifnotdef0) {
  TokenizerTestFile testFile{"directive_ifnotdef0.asm",
                             "directive_ifnotdef0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ifnotdef1) {
  TokenizerTestFile testFile{"directive_ifnotdef1.asm",
                             "directive_ifnotdef1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_incbin0) {
  TokenizerTestFile testFile{"directive_incbin0.asm",
                             "directive_incbin0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_incbin1) {
  TokenizerTestFile testFile{"directive_incbin1.asm",
                             "directive_incbin1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_include0) {
  TokenizerTestFile testFile{"directive_include0.asm",
                             "directive_include0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_include1) {
  TokenizerTestFile testFile{"directive_include1.asm",
                             "directive_include1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_int) {
  TokenizerTestFile testFile{"directive_int.asm", "directive_int.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_irp0) {
  TokenizerTestFile testFile{"directive_irp0.asm", "directive_irp0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_irp1) {
  TokenizerTestFile testFile{"directive_irp1.asm", "directive_irp1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_irpc0) {
  TokenizerTestFile testFile{"directive_irpc0.asm", "directive_irpc0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_irpc1) {
  TokenizerTestFile testFile{"directive_irpc1.asm", "directive_irpc1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_irpc2) {
  TokenizerTestFile testFile{"directive_irpc2.asm", "directive_irpc2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_lcomm0) {
  TokenizerTestFile testFile{"directive_lcomm0.asm", "directive_lcomm0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_lcomm1) {
  TokenizerTestFile testFile{"directive_lcomm1.asm", "directive_lcomm1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_lcomm2) {
  TokenizerTestFile testFile{"directive_lcomm2.asm", "directive_lcomm2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_line0) {
  TokenizerTestFile testFile{"directive_line0.asm", "directive_line0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_line1) {
  TokenizerTestFile testFile{"directive_line1.asm", "directive_line1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_line2) {
  TokenizerTestFile testFile{"directive_line2.asm", "directive_line2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_linkonce0) {
  TokenizerTestFile testFile{"directive_linkonce0.asm",
                             "directive_linkonce0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_linkonce1) {
  TokenizerTestFile testFile{"directive_linkonce1.asm",
                             "directive_linkonce1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_linkonce2) {
  TokenizerTestFile testFile{"directive_linkonce2.asm",
                             "directive_linkonce2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_list) {
  TokenizerTestFile testFile{"directive_list.asm", "directive_list.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ln0) {
  TokenizerTestFile testFile{"directive_ln0.asm", "directive_ln0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_ln1) {
  TokenizerTestFile testFile{"directive_ln1.asm", "directive_ln1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_loc0) {
  TokenizerTestFile testFile{"directive_loc0.asm", "directive_loc0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_loc1) {
  TokenizerTestFile testFile{"directive_loc1.asm", "directive_loc1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_loc2) {
  TokenizerTestFile testFile{"directive_loc2.asm", "directive_loc2.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_loc3) {
  TokenizerTestFile testFile{"directive_loc3.asm", "directive_loc3.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_local0) {
  TokenizerTestFile testFile{"directive_local0.asm", "directive_local0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_local1) {
  TokenizerTestFile testFile{"directive_local1.asm", "directive_local1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_long0) {
  TokenizerTestFile testFile{"directive_long0.asm", "directive_long0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(tokenizer_test_long1) {
  TokenizerTestFile testFile{"directive_long1.asm", "directive_long1.tokens"};
  testFile.doTest();
}
