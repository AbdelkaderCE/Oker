#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <unordered_map>
#include <string>
#include <vector>

enum class ValueType {
    NUMBER,
    STRING,
    BOOLEAN,
    FUNCTION,
    LIST,
    DICTIONARY,
    VOID,
    UNKNOWN
};

struct Symbol {
    std::string name;
    ValueType type;
    bool isFunction;
    std::vector<ValueType> paramTypes;
    ValueType returnType;

    Symbol() : name(""), type(ValueType::UNKNOWN), isFunction(false), returnType(ValueType::UNKNOWN) {}

    Symbol(const std::string& n, ValueType t, bool isFunc = false)
        : name(n), type(t), isFunction(isFunc), returnType(ValueType::UNKNOWN) {}
};

class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;

    Scope(Scope* p = nullptr) : parent(p) {}

    void define(const std::string& name, ValueType type, bool isFunction = false);
    Symbol* lookup(const std::string& name);
    bool exists(const std::string& name);
};

class SemanticAnalyzer {
private:
    Scope* currentScope;
    std::vector<std::unique_ptr<Scope>> scopes;
    ValueType currentFunctionReturnType;
    bool inFunction;
    int loopDepth;

    void enterScope();
    void exitScope();
    void initializeBuiltins();

    ValueType analyzeExpression(Expression* expr);
    void analyzeStatement(Statement* stmt);

    ValueType analyzeBinaryExpression(BinaryExpression* expr);
    ValueType analyzeUnaryExpression(UnaryExpression* expr);
    ValueType analyzeCallExpression(CallExpression* expr);
    ValueType analyzeIdentifier(Identifier* expr);
    ValueType analyzeListLiteral(ListLiteral* expr);
    ValueType analyzeIndexExpression(IndexExpression* expr);
    ValueType analyzeDictLiteral(DictLiteral* expr);

    void analyzeVariableDeclaration(VariableDeclaration* stmt);
    void analyzeAssignment(Assignment* stmt);
    void analyzeFunctionDeclaration(FunctionDeclaration* stmt);
    void analyzeIfStatement(IfStatement* stmt);
    void analyzeWhileStatement(WhileStatement* stmt);
    void analyzeRepeatStatement(RepeatStatement* stmt);
    void analyzeReturnStatement(ReturnStatement* stmt);
    void analyzeBreakStatement(BreakStatement* stmt);
    void analyzeContinueStatement(ContinueStatement* stmt);
    void analyzeExpressionStatement(ExpressionStatement* stmt);

    bool isCompatible(ValueType expected, ValueType actual);
    std::string typeToString(ValueType type);

public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();

    void analyze(Program* program);
};

#endif
