#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>
#include <vector>

// Forward declarations
class ASTNode;
class Expression;
class Statement;

// AST Node types
enum class NodeType {
    PROGRAM,
    VARIABLE_DECLARATION,
    ASSIGNMENT,
    FUNCTION_DECLARATION,
    CLASS_DECLARATION,
    IF_STATEMENT,
    WHILE_STATEMENT,
    REPEAT_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    TRY_STATEMENT,
    EXPRESSION_STATEMENT,
    BLOCK_STATEMENT,
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    INDEX_EXPRESSION, // New
    IDENTIFIER,
    NUMBER_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    LIST_LITERAL,     // New
    DICT_LITERAL
};

class ASTNode {
public:
    NodeType type;
    int line;
    int column;

    ASTNode(NodeType t, int l = 0, int c = 0) : type(t), line(l), column(c) {}
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

class Expression : public ASTNode {
public:
    Expression(NodeType t, int l = 0, int c = 0) : ASTNode(t, l, c) {}
};

class Statement : public ASTNode {
public:
    Statement(NodeType t, int l = 0, int c = 0) : ASTNode(t, l, c) {}
};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;

    Program() : ASTNode(NodeType::PROGRAM) {}
    void print(int indent = 0) const override;
};

class Identifier : public Expression {
public:
    std::string name;

    Identifier(const std::string& n, int l = 0, int c = 0) 
        : Expression(NodeType::IDENTIFIER, l, c), name(n) {}
    void print(int indent = 0) const override;
};

class NumberLiteral : public Expression {
public:
    double value;

    NumberLiteral(double v, int l = 0, int c = 0) 
        : Expression(NodeType::NUMBER_LITERAL, l, c), value(v) {}
    void print(int indent = 0) const override;
};

class StringLiteral : public Expression {
public:
    std::string value;

    StringLiteral(const std::string& v, int l = 0, int c = 0) 
        : Expression(NodeType::STRING_LITERAL, l, c), value(v) {}
    void print(int indent = 0) const override;
};

class BooleanLiteral : public Expression {
public:
    bool value;

    BooleanLiteral(bool v, int l = 0, int c = 0) 
        : Expression(NodeType::BOOLEAN_LITERAL, l, c), value(v) {}
    void print(int indent = 0) const override;
};

class ListLiteral : public Expression {
public:
    std::vector<std::unique_ptr<Expression>> elements;

    ListLiteral(std::vector<std::unique_ptr<Expression>> elems, int l = 0, int c = 0)
        : Expression(NodeType::LIST_LITERAL, l, c), elements(std::move(elems)) {}
    void print(int indent = 0) const override;
};

class IndexExpression : public Expression {
public:
    std::unique_ptr<Expression> object; // The list being indexed
    std::unique_ptr<Expression> index;  // The index to access

    IndexExpression(std::unique_ptr<Expression> obj, std::unique_ptr<Expression> idx, int l = 0, int c = 0)
        : Expression(NodeType::INDEX_EXPRESSION, l, c), object(std::move(obj)), index(std::move(idx)) {}
    void print(int indent = 0) const override;
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    TokenType operator_;

    BinaryExpression(std::unique_ptr<Expression> l, TokenType op, std::unique_ptr<Expression> r, int line = 0, int col = 0)
        : Expression(NodeType::BINARY_EXPRESSION, line, col), left(std::move(l)), right(std::move(r)), operator_(op) {}
    void print(int indent = 0) const override;
};

class UnaryExpression : public Expression {
public:
    std::unique_ptr<Expression> operand;
    TokenType operator_;

    UnaryExpression(TokenType op, std::unique_ptr<Expression> operand, int line = 0, int col = 0)
        : Expression(NodeType::UNARY_EXPRESSION, line, col), operand(std::move(operand)), operator_(op) {}
    void print(int indent = 0) const override;
};

class CallExpression : public Expression {
public:
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;

    CallExpression(std::unique_ptr<Expression> c, int l = 0, int col = 0)
        : Expression(NodeType::CALL_EXPRESSION, l, col), callee(std::move(c)) {}
    void print(int indent = 0) const override;
};

class VariableDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> initializer;

    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> init, int l = 0, int c = 0)
        : Statement(NodeType::VARIABLE_DECLARATION, l, c), name(n), initializer(std::move(init)) {}
    void print(int indent = 0) const override;
};

class Assignment : public Statement {
public:
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;

    Assignment(std::unique_ptr<Expression> t, std::unique_ptr<Expression> v, int l = 0, int c = 0)
        : Statement(NodeType::ASSIGNMENT, l, c), target(std::move(t)), value(std::move(v)) {}
    void print(int indent = 0) const override;
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<Statement>> body;

    FunctionDeclaration(const std::string& n, int l = 0, int c = 0)
        : Statement(NodeType::FUNCTION_DECLARATION, l, c), name(n) {}
    void print(int indent = 0) const override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseBranch;

    IfStatement(std::unique_ptr<Expression> cond, int l = 0, int c = 0)
        : Statement(NodeType::IF_STATEMENT, l, c), condition(std::move(cond)) {}
    void print(int indent = 0) const override;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;

    WhileStatement(std::unique_ptr<Expression> cond, int l = 0, int c = 0)
        : Statement(NodeType::WHILE_STATEMENT, l, c), condition(std::move(cond)) {}
    void print(int indent = 0) const override;
};

class RepeatStatement : public Statement {
public:
    std::unique_ptr<Expression> count;
    std::vector<std::unique_ptr<Statement>> body;

    RepeatStatement(std::unique_ptr<Expression> c, int l = 0, int col = 0)
        : Statement(NodeType::REPEAT_STATEMENT, l, col), count(std::move(c)) {}
    void print(int indent = 0) const override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;

    ReturnStatement(std::unique_ptr<Expression> v, int l = 0, int c = 0)
        : Statement(NodeType::RETURN_STATEMENT, l, c), value(std::move(v)) {}
    void print(int indent = 0) const override;
};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;

    ExpressionStatement(std::unique_ptr<Expression> expr, int l = 0, int c = 0)
        : Statement(NodeType::EXPRESSION_STATEMENT, l, c), expression(std::move(expr)) {}
    void print(int indent = 0) const override;
};

class BreakStatement : public Statement {
public:
    BreakStatement(int l = 0, int c = 0) 
        : Statement(NodeType::BREAK_STATEMENT, l, c) {}
    void print(int indent = 0) const override;
};

class ContinueStatement : public Statement {
public:
    ContinueStatement(int l = 0, int c = 0) 
        : Statement(NodeType::CONTINUE_STATEMENT, l, c) {}
    void print(int indent = 0) const override;
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    Token& peek();
    Token& advance();
    bool match(TokenType type);
    bool check(TokenType type);
    bool isAtEnd();
    void skipNewlines();

    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> logicalOr();
    std::unique_ptr<Expression> logicalAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> addition();
    std::unique_ptr<Expression> multiplication();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();

    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> letStatement();
    std::unique_ptr<Statement> sayStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> repeatStatement();
    std::unique_ptr<Statement> functionDeclaration();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<Statement> breakStatement();
    std::unique_ptr<Statement> continueStatement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> assignmentStatement();

    std::vector<std::unique_ptr<Statement>> block();

public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
};

#endif
