
#include <fstream>
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "tokenizer.h"

BOOST_AUTO_TEST_CASE(test_Tokenizer_isReserved) {
  BOOST_TEST(Tokenizer::isReserved("add") == false);
  BOOST_TEST(Tokenizer::isReserved("73") == false);
  BOOST_TEST(Tokenizer::isReserved(".section") == false);
  BOOST_TEST(Tokenizer::isReserved("EOL") == true);
}

BOOST_AUTO_TEST_CASE(test_Tokenizer_tokenize0) {
  auto tokenizer = Tokenizer{};
  auto input = "add a, 32\n";
  std::stringstream stream;
  stream << input;
  auto tokens = tokenizer.tokenize(stream);
  BOOST_REQUIRE_EQUAL(tokens->size(), 5);
  BOOST_REQUIRE_EQUAL(tokens->at(0), Token("add"));
  BOOST_REQUIRE_EQUAL(tokens->at(1), Token("a"));
  BOOST_REQUIRE_EQUAL(tokens->at(2), Token("32"));
  BOOST_REQUIRE_EQUAL(tokens->at(3), Token("EOL"));
  BOOST_REQUIRE_EQUAL(tokens->at(4), Token("EOF"));
}

class TokenizerTestFile {
  public:
    explicit TokenizerTestFile(std::string filename, std::string expectedFilename) :
      filename(filename), expectedFilename(expectedFilename) {
        stream = std::ifstream{std::string{"test/data/"} + filename};
        expectedStream = std::ifstream{std::string{"test/data/"} + expectedFilename};
    }

    ~TokenizerTestFile() {
      stream.close();
      expectedStream.close();
    }

    void doTest() {
      std::array<char, MAX_LINELEN> line;
      auto tokenizer = Tokenizer{};
      auto tokens = tokenizer.tokenize(stream);
      expectedStream.getline(line.data(), MAX_LINELEN);
      auto token = tokens->begin();
      while ((!expectedStream.eof()) && (token != tokens->end())) {
        BOOST_REQUIRE_EQUAL(std::string{line.data()}, *token);
        expectedStream.getline(line.data(), MAX_LINELEN);
        token++;
      }
      BOOST_TEST(expectedStream.eof());
      if (token != tokens->end()) {
        BOOST_FAIL("token != tokens->end()");
      }
    }

    static const int MAX_LINELEN = 256;

  private:
    std::string filename;
    std::string expectedFilename;
    std::ifstream stream;
    std::ifstream expectedStream;
};

BOOST_AUTO_TEST_CASE(test_Tokenizer_arith0) {
  TokenizerTestFile testFile{"arith0.asm", "arith0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(test_Tokenizer_arith1) {
  TokenizerTestFile testFile{"arith1.asm", "arith1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(test_Tokenizer_comment0) {
  TokenizerTestFile testFile{"comment0.asm", "comment0.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(test_Tokenizer_comment1) {
  TokenizerTestFile testFile{"comment1.asm", "comment1.tokens"};
  testFile.doTest();
}

BOOST_AUTO_TEST_CASE(test_Tokenizer_comment2) {
  TokenizerTestFile testFile{"comment2.asm", "comment2.tokens"};
  testFile.doTest();
}
