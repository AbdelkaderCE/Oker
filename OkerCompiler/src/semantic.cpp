#include "semantic.h"
#include <stdexcept>
#include <iostream>

void Scope::define(const std::string& name, ValueType type, bool isFunction) {
    symbols[name] = Symbol(name, type, isFunction);
}

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}

bool Scope::exists(const std::string& name) {
    return lookup(name) != nullptr;
}

SemanticAnalyzer::SemanticAnalyzer() : currentScope(nullptr), currentFunctionReturnType(ValueType::VOID), inFunction(false), loopDepth(0) {
    scopes.push_back(std::make_unique<Scope>());
    currentScope = scopes.back().get();
    initializeBuiltins();
}

SemanticAnalyzer::~SemanticAnalyzer() = default;

void SemanticAnalyzer::enterScope() {
    scopes.push_back(std::make_unique<Scope>(currentScope));
    currentScope = scopes.back().get();
}

void SemanticAnalyzer::exitScope() {
    if (currentScope && currentScope->parent) {
        currentScope = currentScope->parent;
    }
}

void SemanticAnalyzer::initializeBuiltins() {
    currentScope->define("say", ValueType::FUNCTION, true);
    currentScope->define("input", ValueType::FUNCTION, true);
    currentScope->define("str", ValueType::FUNCTION, true);
    currentScope->define("num", ValueType::FUNCTION, true);
    currentScope->define("bool", ValueType::FUNCTION, true);
    currentScope->define("len", ValueType::FUNCTION, true);
    currentScope->define("type", ValueType::FUNCTION, true);
    currentScope->define("abs", ValueType::FUNCTION, true);
    currentScope->define("random", ValueType::FUNCTION, true);
    currentScope->define("round", ValueType::FUNCTION, true);
    currentScope->define("upper", ValueType::FUNCTION, true);
    currentScope->define("lower", ValueType::FUNCTION, true);
    currentScope->define("strip", ValueType::FUNCTION, true);
    currentScope->define("split_str", ValueType::FUNCTION, true);
    currentScope->define("replace_str", ValueType::FUNCTION, true);
    currentScope->define("list_add", ValueType::FUNCTION, true);
    currentScope->define("exists", ValueType::FUNCTION, true);
    currentScope->define("get", ValueType::FUNCTION, true);
    currentScope->define("save", ValueType::FUNCTION, true);
    currentScope->define("deletef", ValueType::FUNCTION, true);
    currentScope->define("exit", ValueType::FUNCTION, true);
    currentScope->define("sleep", ValueType::FUNCTION, true);
}

void SemanticAnalyzer::analyze(Program* program) {
    for (auto& stmt : program->statements) {
        analyzeStatement(stmt.get());
    }
}

ValueType SemanticAnalyzer::analyzeExpression(Expression* expr) {
    if (!expr) return ValueType::UNKNOWN;
    switch (expr->type) {
        case NodeType::NUMBER_LITERAL: return ValueType::NUMBER;
        case NodeType::STRING_LITERAL: return ValueType::STRING;
        case NodeType::BOOLEAN_LITERAL: return ValueType::BOOLEAN;
        case NodeType::IDENTIFIER: return analyzeIdentifier(static_cast<Identifier*>(expr));
        case NodeType::BINARY_EXPRESSION: return analyzeBinaryExpression(static_cast<BinaryExpression*>(expr));
        case NodeType::UNARY_EXPRESSION: return analyzeUnaryExpression(static_cast<UnaryExpression*>(expr));
        case NodeType::CALL_EXPRESSION: return analyzeCallExpression(static_cast<CallExpression*>(expr));
        case NodeType::LIST_LITERAL: return analyzeListLiteral(static_cast<ListLiteral*>(expr));
        case NodeType::INDEX_EXPRESSION: return analyzeIndexExpression(static_cast<IndexExpression*>(expr));
        case NodeType::DICT_LITERAL: return analyzeDictLiteral(static_cast<DictLiteral*>(expr));
        default: throw std::runtime_error("Unknown expression type in semantic analysis");
    }
}

void SemanticAnalyzer::analyzeStatement(Statement* stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case NodeType::VARIABLE_DECLARATION: analyzeVariableDeclaration(static_cast<VariableDeclaration*>(stmt)); break;
        case NodeType::ASSIGNMENT: analyzeAssignment(static_cast<Assignment*>(stmt)); break;
        case NodeType::FUNCTION_DECLARATION: analyzeFunctionDeclaration(static_cast<FunctionDeclaration*>(stmt)); break;
        case NodeType::IF_STATEMENT: analyzeIfStatement(static_cast<IfStatement*>(stmt)); break;
        case NodeType::WHILE_STATEMENT: analyzeWhileStatement(static_cast<WhileStatement*>(stmt)); break;
        case NodeType::REPEAT_STATEMENT: analyzeRepeatStatement(static_cast<RepeatStatement*>(stmt)); break;
        case NodeType::RETURN_STATEMENT: analyzeReturnStatement(static_cast<ReturnStatement*>(stmt)); break;
        case NodeType::BREAK_STATEMENT: analyzeBreakStatement(static_cast<BreakStatement*>(stmt)); break;
        case NodeType::CONTINUE_STATEMENT: analyzeContinueStatement(static_cast<ContinueStatement*>(stmt)); break;
        case NodeType::EXPRESSION_STATEMENT: analyzeExpressionStatement(static_cast<ExpressionStatement*>(stmt)); break;
        case NodeType::TRY_STATEMENT: analyzeTryStatement(static_cast<TryStatement*>(stmt)); break;
        default: throw std::runtime_error("Unknown statement type in semantic analysis");
    }
}

ValueType SemanticAnalyzer::analyzeBinaryExpression(BinaryExpression* expr) {
    ValueType leftType = analyzeExpression(expr->left.get());
    ValueType rightType = analyzeExpression(expr->right.get());
    switch (expr->operator_) {
        case TokenType::PLUS:
            if (leftType == ValueType::NUMBER && rightType == ValueType::NUMBER) return ValueType::NUMBER;
            if (leftType == ValueType::STRING || rightType == ValueType::STRING) return ValueType::STRING;
            if (leftType == ValueType::UNKNOWN || rightType == ValueType::UNKNOWN) return ValueType::UNKNOWN;
            throw std::runtime_error("Type mismatch in addition");
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
            if ((leftType == ValueType::NUMBER || leftType == ValueType::UNKNOWN) && (rightType == ValueType::NUMBER || rightType == ValueType::UNKNOWN)) return ValueType::NUMBER;
            throw std::runtime_error("Arithmetic on non-numbers");
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS_THAN:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_THAN:
        case TokenType::GREATER_EQUAL:
        case TokenType::AND:
        case TokenType::OR:
            return ValueType::BOOLEAN;
        default: throw std::runtime_error("Unknown binary operator");
    }
}

ValueType SemanticAnalyzer::analyzeUnaryExpression(UnaryExpression* expr) {
    ValueType operandType = analyzeExpression(expr->operand.get());
    switch (expr->operator_) {
        case TokenType::MINUS:
            if (operandType == ValueType::NUMBER || operandType == ValueType::UNKNOWN) return ValueType::NUMBER;
            throw std::runtime_error("Unary minus on non-number");
        case TokenType::NOT:
            if (operandType == ValueType::BOOLEAN || operandType == ValueType::UNKNOWN) return ValueType::BOOLEAN;
            throw std::runtime_error("Logical not on non-boolean");
        default: throw std::runtime_error("Unknown unary operator");
    }
}

ValueType SemanticAnalyzer::analyzeCallExpression(CallExpression* expr) {
    Identifier* callee = dynamic_cast<Identifier*>(expr->callee.get());
    if (!callee) throw std::runtime_error("Can only call functions by name");

    Symbol* symbol = currentScope->lookup(callee->name);
    if (!symbol) throw std::runtime_error("Undefined function: " + callee->name);
    if (!symbol->isFunction) throw std::runtime_error("Cannot call non-function");

    for (auto& arg : expr->arguments) {
        analyzeExpression(arg.get());
    }
    return symbol->returnType;
}

ValueType SemanticAnalyzer::analyzeIdentifier(Identifier* expr) {
    Symbol* symbol = currentScope->lookup(expr->name);
    if (!symbol) throw std::runtime_error("Undefined variable: " + expr->name);
    return symbol->type;
}

ValueType SemanticAnalyzer::analyzeListLiteral(ListLiteral* expr) {
    for (const auto& element : expr->elements) {
        analyzeExpression(element.get());
    }
    return ValueType::LIST;
}

ValueType SemanticAnalyzer::analyzeIndexExpression(IndexExpression* expr) {
    ValueType objectType = analyzeExpression(expr->object.get());
    ValueType indexType = analyzeExpression(expr->index.get());

    if (objectType == ValueType::LIST) {
        if (indexType != ValueType::NUMBER && indexType != ValueType::UNKNOWN) {
            throw std::runtime_error("List index must be a number.");
        }
        return ValueType::UNKNOWN;
    }

    if (objectType == ValueType::DICTIONARY) {
        if (indexType != ValueType::STRING && indexType != ValueType::UNKNOWN) {
            throw std::runtime_error("Dictionary key must be a string.");
        }
        return ValueType::UNKNOWN;
    }

    if (objectType == ValueType::UNKNOWN) {
        return ValueType::UNKNOWN;
    }

    throw std::runtime_error("Cannot index type '" + typeToString(objectType) + "'.");
}

ValueType SemanticAnalyzer::analyzeDictLiteral(DictLiteral* expr) {
    for (size_t i = 0; i < expr->keys.size(); ++i) {
        analyzeExpression(expr->keys[i].get());
        analyzeExpression(expr->values[i].get());
    }
    return ValueType::DICTIONARY;
}

void SemanticAnalyzer::analyzeVariableDeclaration(VariableDeclaration* stmt) {
    ValueType initType = ValueType::UNKNOWN;
    if (stmt->initializer) {
        initType = analyzeExpression(stmt->initializer.get());
    }
    currentScope->define(stmt->name, initType);
}

void SemanticAnalyzer::analyzeAssignment(Assignment* stmt) {
    analyzeExpression(stmt->target.get());
    analyzeExpression(stmt->value.get());
}

void SemanticAnalyzer::analyzeFunctionDeclaration(FunctionDeclaration* stmt) {
    currentScope->define(stmt->name, ValueType::FUNCTION, true);
    Symbol* funcSymbol = currentScope->lookup(stmt->name);

    enterScope();
    for (const auto& param : stmt->parameters) {
        currentScope->define(param, ValueType::UNKNOWN);
    }
    inFunction = true;
    for (auto& bodyStmt : stmt->body) {
        analyzeStatement(bodyStmt.get());
    }
    inFunction = false;
    exitScope();
}

void SemanticAnalyzer::analyzeIfStatement(IfStatement* stmt) {
    analyzeExpression(stmt->condition.get());
    enterScope();
    for (auto& thenStmt : stmt->thenBranch) {
        analyzeStatement(thenStmt.get());
    }
    exitScope();
    if (!stmt->elseBranch.empty()) {
        enterScope();
        for (auto& elseStmt : stmt->elseBranch) {
            analyzeStatement(elseStmt.get());
        }
        exitScope();
    }
}

void SemanticAnalyzer::analyzeWhileStatement(WhileStatement* stmt) {
    analyzeExpression(stmt->condition.get());
    loopDepth++;
    enterScope();
    for (auto& bodyStmt : stmt->body) {
        analyzeStatement(bodyStmt.get());
    }
    exitScope();
    loopDepth--;
}

void SemanticAnalyzer::analyzeRepeatStatement(RepeatStatement* stmt) {
    analyzeExpression(stmt->count.get());
    loopDepth++;
    enterScope();
    for (auto& bodyStmt : stmt->body) {
        analyzeStatement(bodyStmt.get());
    }
    exitScope();
    loopDepth--;
}

void SemanticAnalyzer::analyzeReturnStatement(ReturnStatement* stmt) {
    if (!inFunction) {
        throw std::runtime_error("Return outside function");
    }
    if (stmt->value) {
        analyzeExpression(stmt->value.get());
    }
}

void SemanticAnalyzer::analyzeBreakStatement(BreakStatement* stmt) {
    if (loopDepth == 0) throw std::runtime_error("'break' outside of a loop.");
}

void SemanticAnalyzer::analyzeContinueStatement(ContinueStatement* stmt) {
    if (loopDepth == 0) throw std::runtime_error("'continue' outside of a loop.");
}

void SemanticAnalyzer::analyzeExpressionStatement(ExpressionStatement* stmt) {
    analyzeExpression(stmt->expression.get());
}

void SemanticAnalyzer::analyzeTryStatement(TryStatement* stmt) {
    enterScope();
    for (auto& tryStmt : stmt->tryBlock) {
        analyzeStatement(tryStmt.get());
    }
    exitScope();
    enterScope();
    for (auto& failStmt : stmt->failBlock) {
        analyzeStatement(failStmt.get());
    }
    exitScope();
}

bool SemanticAnalyzer::isCompatible(ValueType expected, ValueType actual) {
    return expected == ValueType::UNKNOWN || actual == ValueType::UNKNOWN || expected == actual;
}

std::string SemanticAnalyzer::typeToString(ValueType type) {
    switch (type) {
        case ValueType::NUMBER: return "number";
        case ValueType::STRING: return "string";
        case ValueType::BOOLEAN: return "boolean";
        case ValueType::FUNCTION: return "function";
        case ValueType::LIST: return "list";
        case ValueType::DICTIONARY: return "dictionary";
        case ValueType::VOID: return "void";
        default: return "unknown";
    }
}