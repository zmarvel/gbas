
#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"


namespace AST {

  class Root;

  template<char reg>
  class Register;

  class Register<'a'>;
  class Register<'f'>;
  class Register<'b'>;
  class Register<'c'>;
  class Register<'d'>;
  class Register<'e'>;
  class Register<'h'>;
  class Register<'l'>;

  template<char reg1, char reg2>
  class DRegister;

  class DRegister<'a', 'f'>;
  class DRegister<'b', 'c'>;
  class DRegister<'d', 'e'>;
  class DRegister<'h', 'l'>;
  class DRegister<'s', 'p'>;
  class DRegister<'p', 'c'>;

  class Label;
  using Number = uint8_t;

  enum class BinaryOp {
    ADD,
    SUB,
    MULT,
    DIV,
  };

  template<BinaryOp op, class Toperand>
  class NumericOp {
    Toperand l;
    Toperand r;
  };

  template<class Toperand>
  using AddOp = NumericOp<BinaryOp::ADD, Toperand>;
  template<class Toperand>
  using SubOp = NumericOp<BinaryOp::SUB, Toperand>;
  template<class Toperand>
  using MultOp = NumericOp<BinaryOp::MULT, Toperand>;
  template<class Toperand>
  using DivOp = NumericOp<BinaryOp::DIV, Toperand>;


  template<int args>
  class Instruction;

  class Instruction<0>;

  template<class T>
  class Instruction1 {
    T operand;
  };
  template<class T>
  class Instruction2 {
    T loperand;
    T roperand;
  };



  class Terminal;



  /* Types of Node:
   * - Node<Instruction<args>>
   * - Node<Register<reg>>
   * - Node<DRegister<reg>>
   * - Node<Label>
   * - Node<Number>
   * - Node<NumericOp<op, T>>
   */
  template<class T>
  class Node {
    Node() {
    };
    ~Node() {
    };
  };

  class Node<Register<'a'>>;
  class Node<Register<'f'>>;
  class Node<Register<'b'>>;
  class Node<Register<'c'>>;
  class Node<Register<'d'>>;
  class Node<Register<'e'>>;
  class Node<Register<'h'>>;
  class Node<Register<'l'>>;


};


class Parser {

  public:
    Parser();
    ~Parser();

    template<class T>
    AST::Node<T> parse(TokenList *tokens, int i);
    
    template<char R>
    AST::Register<R> parseRegister(Token& t);
    
    template<char R1, char R2>
    AST::DRegister<R1, R2> parseDRegister(Token& t);

    AST::Number parseNumber(Token& t);

  private:

};

#endif // PARSER_H
