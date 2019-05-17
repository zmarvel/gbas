
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>

#include "tokenizer.h"


enum class InstructionType {
  ADD,
  ADC,
  INC,
  SUB,
  SBC,
  AND,
  XOR,
  OR,
  CP,
  DEC,
  RLC,
  RLCA,
  RL,
  RLA,
  RRC,
  RRCA,
  RR,
  RRA,
  DAA,
  SCF,
  CPL,
  CCF,

  LD,
  LDI,
  LDD,
  PUSH,
  POP,

  JR,
  RET,
  JP,
  CALL,
  RST,

  NOP,
  STOP,
  HALT,
  DI,
  EI,

  SLA,
  SRA,
  SWAP,
  SRL,
  BIT,
  RES,
  SET,
};

namespace AST {


  /* Types of Node:
   * - Node<Instruction<args>>
   * - Node<Register<reg>>
   * - Node<DRegister<reg>>
   * - Node<Label>
   * - Node<Number>
   * - Node<NumericOp<op, T>>
   */


  enum class NodeType {
    ROOT,
    INSTRUCTION,
    REGISTER,
    DREGISTER,
    LABEL,
    NUMBER,
    NUMERIC_OP,
    INVALID,
  };

  struct BaseNode {
    virtual NodeType id() const {
      return NodeType::INVALID;
    };
  };

  template<NodeType Tnode>
  struct Node : BaseNode {
    virtual NodeType id() const override {
      return Tnode;
    }
  };

  class Root : public Node<NodeType::ROOT> {
    public:
      Root() :
        mChildren{}
      { }

      void addChild(BaseNode child) {
        mChildren.push_back(child);
      }

      std::vector<BaseNode>::iterator begin() {
        return mChildren.begin();
      }

      std::vector<BaseNode>::iterator end() {
        return mChildren.end();
      }

      size_t nChildren() const {
        return mChildren.size();
      }

    private:
      std::vector<BaseNode> mChildren;
  };


  class RegisterBase : public Node<NodeType::REGISTER> {
    public:
      virtual char reg() const = 0;
  };

  template<char regc>
  struct Register : public RegisterBase {
    public:
      virtual char reg() const override {
        return regc;
      }
  };

  class DRegisterBase : public Node<NodeType::DREGISTER> {
    public:
      virtual std::string reg() const = 0;
  };

  template<char reg1, char reg2>
  struct DRegister : public DRegisterBase {
    virtual std::string reg() const override {
      return std::string{reg1, reg2};
    }
  };

  class Label : public Node<NodeType::LABEL> {
  };

  struct Number : public Node<NodeType::NUMBER> {
      explicit Number(uint8_t value) :
        mValue{value}
      {
      }

    private:
      uint8_t mValue;
  };

  enum class BinaryOp {
    ADD,
    SUB,
    MULT,
    DIV,
  };

  template<BinaryOp Top>
  class NumericOp : public Node<NodeType::NUMERIC_OP> {
    public:
      explicit NumericOp(BaseNode l, BaseNode r) :
        mL{l}, mR{r}
      {
      }

      constexpr BinaryOp getOpType() {
        return Top;
      }
    private:
      BaseNode mL, mR;
  };

  using AddOp = NumericOp<BinaryOp::ADD>;
  using SubOp = NumericOp<BinaryOp::SUB>;
  using MultOp = NumericOp<BinaryOp::MULT>;
  using DivOp = NumericOp<BinaryOp::DIV>;

  class Instruction0 : public Node<NodeType::INSTRUCTION> {
    public:
      Instruction0(InstructionType type) :
        mType{type}
      { }

      virtual InstructionType type() {
        return mType;
      }

    private:
      InstructionType mType;
  };

  class Instruction1 : public Node<NodeType::INSTRUCTION> {
    public:
      Instruction1(InstructionType type, BaseNode operand) :
        mType{type},
        mOperand{operand}
      { }

      virtual InstructionType type() {
        return mType;
      }

      BaseNode operand() {
        return mOperand;
      }

    private:
      InstructionType mType;
      BaseNode mOperand;
  };

  class Instruction2 : public Node<NodeType::INSTRUCTION> {
    public:
      Instruction2(InstructionType type, BaseNode loperand, BaseNode roperand) :
        mType{type},
        mLoperand{loperand},
        mRoperand{roperand}
      { }

      virtual InstructionType type() {
        return mType;
      }

      BaseNode loperand() {
        return mLoperand;
      }

      BaseNode roperand() {
        return mRoperand;
      }

    private:
      InstructionType mType;
      BaseNode mLoperand;
      BaseNode mRoperand;

  };

  class Terminal;

};


struct InstructionProps {
  const std::string lexeme;
  InstructionType type;
  int args1;
  int args2;
};

using InstructionPropsList = const std::array<const InstructionProps, 22+5+5+5+7>;

class Parser {

  public:
    Parser();
    ~Parser();

    std::shared_ptr<AST::BaseNode> parse(TokenList& tokens, int i,
                                         AST::BaseNode prev = AST::BaseNode{});
    
    std::shared_ptr<AST::BaseNode> parseRegister(const Token& t);
    
    std::shared_ptr<AST::DRegisterBase> parseDRegister(const Token& t);

    std::shared_ptr<AST::BaseNode> parseNumber(const Token& t);

    std::shared_ptr<AST::BaseNode> parseNumericOp(AST::BaseNode lrand, const Token& t,
                                 AST::BaseNode rrand);
    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok);
    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok, AST::BaseNode rand);
    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok, AST::BaseNode lrand,
                                   AST::BaseNode rrand);
    std::shared_ptr<AST::BaseNode> parseLabel(const Token& t);

    static bool isNumber(const Token& tok);
    static bool isNumericOp(const Token& tok);
    static InstructionPropsList::const_iterator findInstruction(const Token& tok);
    static bool isInstruction(const Token& tok);
    static bool isLabel(const Token& tok);
    static bool isRegister(const Token& tok);
    static bool isDRegister(const Token& tok);


  private:
    AST::Root mRoot;

};

class ParserException : std::exception {
  public:
    ParserException(const char* msg) {
      mMsg = msg;
    }
    
    ParserException(const std::string& msg) {
      mMsg = msg;
    }

    virtual const char* what() const noexcept {
      return mMsg.c_str();
    }

  private:
    std::string mMsg;
};

#endif // PARSER_H
