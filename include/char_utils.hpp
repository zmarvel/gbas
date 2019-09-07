
#ifndef CHAR_UTILS_HPP
#define CHAR_UTILS_HPP

#include <algorithm>

namespace GBAS {

constexpr bool isAlpha(char c) {
  return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

constexpr bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

constexpr bool isAlphaNumeric(char c) {
  return isAlpha(c) || isDigit(c);
}

static inline bool isNumber(const std::string tok) {
  if (tok.size() == 0) {
    return false;
  }

  for (auto it = tok.begin(); it != tok.end(); it++) {
    if (!isDigit(*it)) {
      return false;
    }
  }
  return true;
}

constexpr bool isNumericOp(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/';
}

static inline bool isMaybeSection(const std::string& tok) {
  if (tok.size() == 0) {
    return false;
  } else if (!isAlpha(tok[0])) {
    return false;
  }

  return std::all_of(tok.begin() + 1, tok.end(),
      [](auto c) { return isAlphaNumeric(c) || c == '_' || c == '.'; });
}

}; // namespace GBAS

#endif // CHAR_UTILS_HPP
