#include "semantic.h"
#include <stdexcept>
#include <iostream>

// --- Scope Implementation ---
void Scope::define(const std::string& name, std::unique_ptr<Symbol> symbol) {
    symbols[name] = std::move(symbol);
}

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}

bool Scope::exists(const std::string& name) {
    return lookup(name) != nullptr;
}


// --- SemanticAnalyzer Implementation ---
SemanticAnalyzer::SemanticAnalyzer() : currentScope(nullptr), currentFunctionReturnType(ValueType::VOID), inFunction(false), loopDepth(0), currentClass(nullptr) {
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
    auto builtins = {
        "say", "input", "str", "num", "bool", "len", "type", "abs", "random",
        "round", "upper", "lower", "strip", "split_str", "replace_str",
        "list_add", "exists", "get", "save", "deletef", "exit", "sleep"
    };
    for (const auto& name : builtins) {
        currentScope->define(name, std::make_unique<Symbol>(name, ValueType::FUNCTION, true));
    }
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
        case NodeType::CALL_EXPRESSION:
            // Check if this is a constructor call
            if (dynamic_cast<CallExpression*>(expr)->callee->type == NodeType::IDENTIFIER) {
                 return analyzeCallExpression(static_cast<CallExpression*>(expr));
            }
            // This is a special case for 'new' expressions, which are parsed as CallExpressions
            return analyzeNewExpression(static_cast<CallExpression*>(expr));
        case NodeType::LIST_LITERAL: return analyzeListLiteral(static_cast<ListLiteral*>(expr));
        case NodeType::INDEX_EXPRESSION: return analyzeIndexExpression(static_cast<IndexExpression*>(expr));
        case NodeType::DICT_LITERAL: return analyzeDictLiteral(static_cast<DictLiteral*>(expr));
        case NodeType::MEMBER_EXPRESSION: return analyzeMemberExpression(static_cast<MemberExpression*>(expr));
        default: throw std::runtime_error("Unknown expression type in semantic analysis");
    }
}

void SemanticAnalyzer::analyzeStatement(Statement* stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case NodeType::VARIABLE_DECLARATION: analyzeVariableDeclaration(static_cast<VariableDeclaration*>(stmt)); break;
        case NodeType::ASSIGNMENT: analyzeAssignment(static_cast<Assignment*>(stmt)); break;
        case NodeType::FUNCTION_DECLARATION: analyzeFunctionDeclaration(static_cast<FunctionDeclaration*>(stmt)); break;
        case NodeType::CLASS_DECLARATION: analyzeClassDeclaration(static_cast<ClassDeclaration*>(stmt)); break;
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
    if (!callee) { // This handles method calls like `my_instance.my_method()`
         if (expr->callee->type == NodeType::MEMBER_EXPRESSION) {
            return analyzeMemberExpression(static_cast<MemberExpression*>(expr->callee.get()));
        }
        throw std::runtime_error("Can only call functions by name or method");
    }


    Symbol* symbol = currentScope->lookup(callee->name);
    if (!symbol) throw std::runtime_error("Undefined function: " + callee->name);
    if (!symbol->isFunction) throw std::runtime_error("Cannot call non-function");

    for (auto& arg : expr->arguments) {
        analyzeExpression(arg.get());
    }
    return symbol->returnType;
}

ValueType SemanticAnalyzer::analyzeIdentifier(Identifier* expr) {
    if (expr->name == "this" && currentClass) {
        return ValueType::INSTANCE;
    }
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

// New function to analyze member access
ValueType SemanticAnalyzer::analyzeMemberExpression(MemberExpression* expr) {
    ValueType objectType = analyzeExpression(expr->object.get());
    if (objectType != ValueType::INSTANCE) {
        throw std::runtime_error("Member access ('.') is only valid on class instances.");
    }
    // Further analysis would require knowing the class of the instance
    // to check if the property exists. For now, we assume it's valid.
    return ValueType::UNKNOWN; // The type of the property is unknown
}

// New function to analyze constructor calls
ValueType SemanticAnalyzer::analyzeNewExpression(CallExpression* expr) {
    Identifier* className = dynamic_cast<Identifier*>(expr->callee.get());
    if (!className) {
        throw std::runtime_error("Expected a class name after 'new'.");
    }

    Symbol* symbol = currentScope->lookup(className->name);
    if (!symbol || symbol->type != ValueType::CLASS) {
        throw std::runtime_error("'" + className->name + "' is not a defined class.");
    }

    // Analyze arguments if there is a constructor
    // For now, just analyze the expressions
    for (const auto& arg : expr->arguments) {
        analyzeExpression(arg.get());
    }

    return ValueType::INSTANCE;
}

void SemanticAnalyzer::analyzeVariableDeclaration(VariableDeclaration* stmt) {
    ValueType initType = ValueType::UNKNOWN;
    if (stmt->initializer) {
        initType = analyzeExpression(stmt->initializer.get());
    }
    currentScope->define(stmt->name, std::make_unique<Symbol>(stmt->name, initType));
}

void SemanticAnalyzer::analyzeAssignment(Assignment* stmt) {
    analyzeExpression(stmt->target.get());
    analyzeExpression(stmt->value.get());
}

void SemanticAnalyzer::analyzeFunctionDeclaration(FunctionDeclaration* stmt) {
    currentScope->define(stmt->name, std::make_unique<Symbol>(stmt->name, ValueType::FUNCTION, true));

    enterScope();
    for (const auto& param : stmt->parameters) {
        currentScope->define(param, std::make_unique<Symbol>(param, ValueType::UNKNOWN));
    }
    inFunction = true;
    for (auto& bodyStmt : stmt->body) {
        analyzeStatement(bodyStmt.get());
    }
    inFunction = false;
    exitScope();
}

// New function to analyze class declarations
void SemanticAnalyzer::analyzeClassDeclaration(ClassDeclaration* stmt) {
    auto classSymbol = std::make_unique<ClassSymbol>(stmt->name);
    currentClass = classSymbol.get();

    enterScope();
    for (const auto& method : stmt->methods) {
        auto methodSymbol = std::make_unique<Symbol>(method->name, ValueType::FUNCTION, true);
        // In a more complex system, you'd analyze parameter types here
        classSymbol->methods[method->name] = std::move(methodSymbol);
        analyzeFunctionDeclaration(method.get());
    }
    exitScope();

    currentScope->define(stmt->name, std::move(classSymbol));
    currentClass = nullptr;
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
        case ValueType::CLASS: return "class";
        case ValueType::INSTANCE: return "instance";
        case ValueType::LIST: return "list";
        case ValueType::DICTIONARY: return "dictionary";
        case ValueType::VOID: return "void";
        default: return "unknown";
    }
}