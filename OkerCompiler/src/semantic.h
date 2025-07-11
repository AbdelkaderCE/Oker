#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

enum class ValueType {
    NUMBER,
    STRING,
    BOOLEAN,
    FUNCTION,
    CLASS,      // Represents the class definition itself
    INSTANCE,   // Represents an instance of a class
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

// New structure to hold information about a class
struct ClassSymbol : public Symbol {
    std::unordered_map<std::string, std::unique_ptr<Symbol>> methods;

    ClassSymbol(const std::string& n) : Symbol(n, ValueType::CLASS, false) {}
};


class Scope {
public:
    std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols;
    Scope* parent;

    Scope(Scope* p = nullptr) : parent(p) {}

    void define(const std::string& name, std::unique_ptr<Symbol> symbol);
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
    ClassSymbol* currentClass = nullptr; // Track the current class context

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
    ValueType analyzeMemberExpression(MemberExpression* expr); // Added
    ValueType analyzeNewExpression(CallExpression* expr); // Added for 'new'

    void analyzeVariableDeclaration(VariableDeclaration* stmt);
    void analyzeAssignment(Assignment* stmt);
    void analyzeFunctionDeclaration(FunctionDeclaration* stmt);
    void analyzeClassDeclaration(ClassDeclaration* stmt); // Added
    void analyzeIfStatement(IfStatement* stmt);
    void analyzeWhileStatement(WhileStatement* stmt);
    void analyzeRepeatStatement(RepeatStatement* stmt);
    void analyzeReturnStatement(ReturnStatement* stmt);
    void analyzeBreakStatement(BreakStatement* stmt);
    void analyzeContinueStatement(ContinueStatement* stmt);
    void analyzeExpressionStatement(ExpressionStatement* stmt);
    void analyzeTryStatement(TryStatement* stmt);

    bool isCompatible(ValueType expected, ValueType actual);
    std::string typeToString(ValueType type);

public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();

    void analyze(Program* program);
};

#endif