
#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <memory>

#include "tokenizer.hpp"

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
    BINARY_OP,
    UNARY_OP,
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

      Root(std::vector<std::shared_ptr<BaseNode>> children) :
        mChildren{children}
      { }

      void add(std::shared_ptr<BaseNode> child) {
        mChildren.push_back(child);
      }

      BaseNode& child(size_t i) {
        return *mChildren.at(i);
      }

      std::vector<std::shared_ptr<BaseNode>>::iterator begin() {
        return mChildren.begin();
      }

      std::vector<std::shared_ptr<BaseNode>>::iterator end() {
        return mChildren.end();
      }

      size_t size() const {
        return mChildren.size();
      }

    private:
      std::vector<std::shared_ptr<BaseNode>> mChildren;
  };


  class BaseRegister : public Node<NodeType::REGISTER> {
    public:
      virtual char reg() const = 0;
  };

  template<char regc>
  struct Register : public BaseRegister {
    public:
      virtual char reg() const override {
        return regc;
      }
  };

  class BaseDRegister : public Node<NodeType::DREGISTER> {
    public:
      virtual std::string reg() const = 0;
  };

  template<char reg1, char reg2>
  struct DRegister : public BaseDRegister {
    virtual std::string reg() const override {
      return std::string{reg1, reg2};
    }
  };

  class Label : public Node<NodeType::LABEL> {
    public:
      Label(const std::string& name) :
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

  enum class BinaryOpType {
    ADD,
    SUB,
    MULT,
    DIV,
    INVALID,
  };

  class BaseBinaryOp : public Node<NodeType::BINARY_OP> {
    public:
      virtual BinaryOpType opType() const {
        return BinaryOpType::INVALID;
      }

      std::shared_ptr<BaseNode> left() {
        return mL;
      }

      std::shared_ptr<BaseNode> right() {
        return mR;
      }

    protected:
      explicit BaseBinaryOp(std::shared_ptr<BaseNode> l,
                        std::shared_ptr<BaseNode> r) :
        mL{l}, mR{r}
      { }

    private:
      std::shared_ptr<BaseNode> mL, mR;
  };

  template<BinaryOpType Top>
  class BinaryOp : public BaseBinaryOp {
    public:
      explicit BinaryOp(std::shared_ptr<BaseNode> l,
                        std::shared_ptr<BaseNode> r) :
        BaseBinaryOp{l, r}
      { }

      virtual BinaryOpType opType() const {
        return Top;
      }
  };

  using AddOp = BinaryOp<BinaryOpType::ADD>;
  using SubOp = BinaryOp<BinaryOpType::SUB>;
  using MultOp = BinaryOp<BinaryOpType::MULT>;
  using DivOp = BinaryOp<BinaryOpType::DIV>;

  enum class UnaryOpType {
    NEG,
    INVALID,
  };

  class BaseUnaryOp : public Node<NodeType::UNARY_OP> {
    public:
      virtual UnaryOpType opType() const {
        return UnaryOpType::INVALID;
      }

      std::shared_ptr<BaseNode> operand() {
        return mRand;
      }

    protected:
      explicit BaseUnaryOp(std::shared_ptr<BaseNode> rand) :
        mRand{rand}
      { }

    private:
      std::shared_ptr<BaseNode> mRand;

  };

  template<UnaryOpType Top>
  class UnaryOp : public BaseUnaryOp {
    public:
      explicit UnaryOp(std::shared_ptr<BaseNode> rand) :
        BaseUnaryOp{rand}
      { }

      virtual UnaryOpType opType() const {
        return Top;
      }
  };

  using NegOp = UnaryOp<UnaryOpType::NEG>;

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

      std::shared_ptr<BaseNode> operand() {
        return mOperand;
      }

    private:
      std::shared_ptr<BaseNode> mOperand;
  };

  class Instruction2 : public BaseInstruction {
    public:
      Instruction2(InstructionType type, std::shared_ptr<BaseNode> left,
                   std::shared_ptr<BaseNode> right) :
        BaseInstruction{type, 2},
        mLoperand{left},
        mRoperand{right}
      { }

      std::shared_ptr<BaseNode> left() {
        return mLoperand;
      }

      std::shared_ptr<BaseNode> right() {
        return mRoperand;
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


/*
 * program → line* EOF ;
 *
 * line → instruction | label | directive ;
 *
 * instruction → instruction0 | instruction1 | instruction2 ;
 * instruction0 → INSTRUCTION newline ;
 * instruction1 → INSTRUCTION operand newline ;
 * instruction2 → INSTRUCTION operand "," operand newline ;
 *
 * operand → REGISTER | DREGISTER | addition ;
 * addition → multiplication ( ( "+" | "-" ) multiplication )* ;
 * multiplication → unary ( ( "*" | "/" ) unary )* ;
 * unary → "-" unary | primary ;
 * primary → NUMBER | LABEL ;
 *
 *
 * label → LABEL ":" ;
 * 
 * directive → "." DIRECTIVE ;
 *
 * newline → "EOL"
 */


class Parser {

  public:
    Parser(TokenList& tokens);
    ~Parser();

    /**
     * Convert TokenList to an AST.
     * 
     * @param tokens: Input list of tokens.
     * @param i: Current position in the list (allowing recursive calls).
     */
    std::shared_ptr<AST::BaseNode> parse();

    /**
     * Return the next Token in the list and increment the position counter.
     * Throws an exception when there are no more Tokens.
     */
    Token next();

    /**
     * Return the next Token in the list without incrementing the position
     * counter. Throws an exception when there are no more Tokens.
     */
    Token peek();

    /**
     * Return the "next next" Token in the list without incrementing the
     * position counter. Throws an exception when there are no more Tokens.
     */
    Token peekNext();

    std::shared_ptr<AST::BaseNode> program();

    std::shared_ptr<AST::BaseNode> line();
    std::shared_ptr<AST::BaseNode> instruction();
    std::shared_ptr<AST::BaseNode> operand();
    std::shared_ptr<AST::BaseNode> register_();
    std::shared_ptr<AST::BaseNode> dregister();
    std::shared_ptr<AST::BaseNode> addition();
    bool isAddition(const Token& tok);
    std::shared_ptr<AST::BaseNode> multiplication();
    bool isMultiplication(const Token& tok);
    std::shared_ptr<AST::BaseNode> unary();
    std::shared_ptr<AST::BaseNode> primary();
    std::shared_ptr<AST::BaseNode> label();
    std::shared_ptr<AST::BaseNode> number();

    /**
     * Read from tokens, starting at pos, until EOF is encountered.
     */
    TokenList readLine(TokenList& tokens);
    
    /**
     * Convert tok to a Register node, or raise an exception if this is not
     * possible.
     */
    std::shared_ptr<AST::BaseRegister> parseRegister(const Token& tok);
    
    /**
     * Convert tok to a DRegister node, or raise an exception if this is not
     * possible.
     */
    std::shared_ptr<AST::BaseDRegister> parseDRegister(const Token& tok);

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

    std::shared_ptr<AST::BaseNode> parseInstruction(const Token& tok, const TokenList& rands);

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
    TokenList mTokens;
    AST::Root mRoot;
    int mPos;
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
