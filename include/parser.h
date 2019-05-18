
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>

#include "tokenizer.h"

namespace AST {

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

    INVALID,
  };



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

      void add(BaseNode child) {
        mChildren.push_back(child);
      }

      BaseNode& child(size_t i) {
        return mChildren.at(i);
      }

      std::vector<BaseNode>::iterator begin() {
        return mChildren.begin();
      }

      std::vector<BaseNode>::iterator end() {
        return mChildren.end();
      }

      size_t size() const {
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
    public:
      Label(std::string(name)) :
        mName{name}
      { }

      const std::string& name() const {
        return mName;
      }

    private:
      std::string mName;
  };

  struct Number : public Node<NodeType::NUMBER> {
      explicit Number(uint8_t value) :
        mValue{value}
      {
      }

      uint8_t value() const {
        return mValue;
      }

    private:
      uint8_t mValue;
  };

  enum class BinaryOp {
    ADD,
    SUB,
    MULT,
    DIV,
    INVALID,
  };

  class BaseNumericOp : public Node<NodeType::NUMERIC_OP> {
    public:
      virtual BinaryOp opType() const {
        return BinaryOp::INVALID;
      }
  };

  template<BinaryOp Top>
  class NumericOp : public BaseNumericOp {
    public:
      explicit NumericOp(std::shared_ptr<BaseNode> l, std::shared_ptr<BaseNode> r) :
        mL{l}, mR{r}
      {
      }

      virtual BinaryOp opType() const {
        return Top;
      }

      BaseNode& left() {
        return *mL;
      }

      BaseNode& right() {
        return *mR;
      }

    private:
      std::shared_ptr<BaseNode> mL, mR;
  };

  using AddOp = NumericOp<BinaryOp::ADD>;
  using SubOp = NumericOp<BinaryOp::SUB>;
  using MultOp = NumericOp<BinaryOp::MULT>;
  using DivOp = NumericOp<BinaryOp::DIV>;

  class BaseInstruction : public Node<NodeType::INSTRUCTION> {
    public:
      BaseInstruction() :
        mType{InstructionType::INVALID},
        mNOperands{0}
      { }

      BaseInstruction(InstructionType type, int nOperands) :
        mType{type},
        mNOperands{nOperands}
      { }

      InstructionType type() const {
        return mType;
      }

      int nOperands() const {
        return mNOperands;
      }

    protected: 
      InstructionType mType;
      int mNOperands;
  };

  class Instruction0 : public BaseInstruction {
    public:
      Instruction0(InstructionType type) :
        BaseInstruction{type, 0}
      { }

    private:
      InstructionType mType;
  };

  class Instruction1 : public BaseInstruction {
    public:
      Instruction1(InstructionType type, std::shared_ptr<BaseNode> operand) :
        BaseInstruction{type, 1},
        mOperand{operand}
      { }

      BaseNode& operand() {
        return *mOperand;
      }

    private:
      std::shared_ptr<BaseNode> mOperand;
  };

  class Instruction2 : public BaseInstruction {
    public:
      Instruction2(InstructionType type, std::shared_ptr<BaseNode> loperand,
                   std::shared_ptr<BaseNode> roperand) :
        BaseInstruction{type, 2},
        mLoperand{loperand},
        mRoperand{roperand}
      { }

      BaseNode& loperand() {
        return *mLoperand;
      }

      BaseNode& roperand() {
        return *mRoperand;
      }

    private:
      InstructionType mType;
      std::shared_ptr<BaseNode> mLoperand, mRoperand;

  };

  class Terminal;

};


struct InstructionProps {
  const std::string lexeme;
  AST::InstructionType type;
  int args1;
  int args2;
};

using InstructionPropsList = const std::array<const InstructionProps, 22+5+5+5+7>;

class Parser {

  public:
    Parser();
    ~Parser();

    /**
     * Convert TokenList to an AST.
     * 
     * @param tokens: Input list of tokens.
     * @param i: Current position in the list (allowing recursive calls).
     */
    std::shared_ptr<AST::Root> parse(TokenList& tokens);
    
    /**
     * Convert tok to a Register node, or raise an exception if this is not
     * possible.
     */
    std::shared_ptr<AST::RegisterBase> parseRegister(const Token& tok);
    
    /**
     * Convert tok to a DRegister node, or raise an exception if this is not
     * possible.
     */
    std::shared_ptr<AST::DRegisterBase> parseDRegister(const Token& tok);

    /**
     * Convert tok to a Number node, or raise an exception if this is not
     * possible.
     */
    std::shared_ptr<AST::BaseNode> parseNumber(const Token& tok);

    /**
     * Convert tok to a NumericOp node, with lrand and rrand as its respective
     * operands.
     */
    std::shared_ptr<AST::BaseNode> parseNumericOp(std::shared_ptr<AST::BaseNode> lrand,
                                                  const Token& tok,
                                                  std::shared_ptr<AST::BaseNode> rrand);

    /**
     * Convert tok to an Instruction0. If this is not possible, raise an
     * exception.
     */
    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok);

    /**
     * Convert tok to an Instruction1, with rand as its parameter. If this is
     * not possible, raise an exception.
     */
    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok,
                                                    std::shared_ptr<AST::BaseNode> rand);
    /**
     * Convert tok to an Instruction2, with rand1 and rand2 as its parameters.
     * If this is not possible, raise an exception.
     */
    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok,
                                                    std::shared_ptr<AST::BaseNode> rand1,
                                                    std::shared_ptr<AST::BaseNode> rand2);

    /**
     * Convert tok to a Label node. If this is not possible, raise an exception.
     */
    std::shared_ptr<AST::BaseNode> parseLabel(const Token& t);

    /**
     * Convert tok to a Register, Number, or Label node. If this is not
     * possible, raise an exception.
     */
    std::shared_ptr<AST::BaseNode> parseOperand(const Token& tok);

    /**
     * True only if tok is an integer number.
     */
    static bool isNumber(const Token& tok);

    /**
     * True only if tok is one of +, -, *, or /.
     */
    static bool isNumericOp(const Token& tok);

    /**
     * Search the InstructionPropsList for properties associated with the
     * instruction tok.
     */
    static InstructionPropsList::const_iterator findInstruction(const Token& tok);
    
    /**
     * True only if tok is a valid instruction name.
     */
    static bool isInstruction(const Token& tok);

    /**
     * True only if tok starts with a letter and, following that, contains only
     * letters, numbers, and underscores.
     */
    static bool isLabel(const Token& tok);

    /**
     * True only if tok is one of a, f, b, c, d, e, h, l. 
     */
    static bool isRegister(const Token& tok);

    /**
     * True only if tok is one of af, bc, de, hl, sp, or pc.
     */
    static bool isDRegister(const Token& tok);

    /**
     * True only if tok is EOL.
     */
    static bool isNewline(const Token& tok);

    /**
     * True only if tok is EOF.
     */
    static bool isEof(const Token& tok);

    /**
     * Expect a newline in the provided TokenList in [start, start+max). If no
     * newline is found in the provided range, return -1.
     */
    static int expectNewline(const TokenList& list, int start, int max);


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
