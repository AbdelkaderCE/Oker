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
    // Create global scope
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
    currentScope->define("max", ValueType::FUNCTION, true);
    currentScope->define("min", ValueType::FUNCTION, true);
    currentScope->define("round", ValueType::FUNCTION, true);
    currentScope->define("sqrt", ValueType::FUNCTION, true);
    currentScope->define("pow", ValueType::FUNCTION, true);
    currentScope->define("random", ValueType::FUNCTION, true);
    currentScope->define("upper", ValueType::FUNCTION, true);
    currentScope->define("lower", ValueType::FUNCTION, true);
    currentScope->define("strip", ValueType::FUNCTION, true);
    currentScope->define("split_str", ValueType::FUNCTION, true);
    currentScope->define("join", ValueType::FUNCTION, true);
    currentScope->define("replace_str", ValueType::FUNCTION, true);
    currentScope->define("charAt", ValueType::FUNCTION, true);
    currentScope->define("sbuild_new", ValueType::FUNCTION, true);
    currentScope->define("sbuild_add", ValueType::FUNCTION, true);
    currentScope->define("sbuild_get", ValueType::FUNCTION, true);
    currentScope->define("list_add", ValueType::FUNCTION, true);
    currentScope->define("exists", ValueType::FUNCTION, true);
    currentScope->define("listdir", ValueType::FUNCTION, true);
    currentScope->define("exit", ValueType::FUNCTION, true);
    currentScope->define("sleep", ValueType::FUNCTION, true);
    currentScope->define("get", ValueType::FUNCTION, true);
    currentScope->define("save", ValueType::FUNCTION, true);
    currentScope->define("deletef", ValueType::FUNCTION, true);
}

void SemanticAnalyzer::analyze(Program* program) {
    for (auto& stmt : program->statements) {
        analyzeStatement(stmt.get());
    }
}

ValueType SemanticAnalyzer::analyzeExpression(Expression* expr) {
    switch (expr->type) {
        case NodeType::NUMBER_LITERAL:
            return ValueType::NUMBER;

        case NodeType::STRING_LITERAL:
            return ValueType::STRING;

        case NodeType::BOOLEAN_LITERAL:
            return ValueType::BOOLEAN;

        case NodeType::IDENTIFIER:
            return analyzeIdentifier(static_cast<Identifier*>(expr));

        case NodeType::BINARY_EXPRESSION:
            return analyzeBinaryExpression(static_cast<BinaryExpression*>(expr));

        case NodeType::UNARY_EXPRESSION:
            return analyzeUnaryExpression(static_cast<UnaryExpression*>(expr));

        case NodeType::CALL_EXPRESSION:
            return analyzeCallExpression(static_cast<CallExpression*>(expr));

        case NodeType::LIST_LITERAL:
            return analyzeListLiteral(static_cast<ListLiteral*>(expr));

        case NodeType::INDEX_EXPRESSION:
            return analyzeIndexExpression(static_cast<IndexExpression*>(expr));
        case NodeType::DICT_LITERAL:
            return analyzeDictLiteral(static_cast<DictLiteral*>(expr));

        default:
            throw std::runtime_error("Unknown expression type in semantic analysis");
    }
}

void SemanticAnalyzer::analyzeStatement(Statement* stmt) {
    switch (stmt->type) {
        case NodeType::VARIABLE_DECLARATION:
            analyzeVariableDeclaration(static_cast<VariableDeclaration*>(stmt));
            break;

        case NodeType::ASSIGNMENT:
            analyzeAssignment(static_cast<Assignment*>(stmt));
            break;

        case NodeType::FUNCTION_DECLARATION:
            analyzeFunctionDeclaration(static_cast<FunctionDeclaration*>(stmt));
            break;

        case NodeType::IF_STATEMENT:
            analyzeIfStatement(static_cast<IfStatement*>(stmt));
            break;

        case NodeType::WHILE_STATEMENT:
            analyzeWhileStatement(static_cast<WhileStatement*>(stmt));
            break;

        case NodeType::REPEAT_STATEMENT:
            analyzeRepeatStatement(static_cast<RepeatStatement*>(stmt));
            break;

        case NodeType::RETURN_STATEMENT:
            analyzeReturnStatement(static_cast<ReturnStatement*>(stmt));
            break;

        case NodeType::BREAK_STATEMENT:
            analyzeBreakStatement(static_cast<BreakStatement*>(stmt));
            break;

        case NodeType::CONTINUE_STATEMENT:
            analyzeContinueStatement(static_cast<ContinueStatement*>(stmt));
            break;

        case NodeType::EXPRESSION_STATEMENT:
            analyzeExpressionStatement(static_cast<ExpressionStatement*>(stmt));
            break;

        default:
            throw std::runtime_error("Unknown statement type in semantic analysis");
    }
}

ValueType SemanticAnalyzer::analyzeBinaryExpression(BinaryExpression* expr) {
    ValueType leftType = analyzeExpression(expr->left.get());
    ValueType rightType = analyzeExpression(expr->right.get());

    switch (expr->operator_) {
        case TokenType::PLUS:
            if (leftType == ValueType::NUMBER && rightType == ValueType::NUMBER) {
                return ValueType::NUMBER;
            }
            if (leftType == ValueType::STRING || rightType == ValueType::STRING) {
                return ValueType::STRING;
            }
            if (leftType == ValueType::UNKNOWN || rightType == ValueType::UNKNOWN) {
                return ValueType::UNKNOWN;
            }
            throw std::runtime_error("Type mismatch in addition operation");

        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
            if ((leftType == ValueType::NUMBER || leftType == ValueType::UNKNOWN) && 
                (rightType == ValueType::NUMBER || rightType == ValueType::UNKNOWN)) {
                return ValueType::NUMBER;
            }
            throw std::runtime_error("Arithmetic operations require numeric operands");

        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
            return ValueType::BOOLEAN;

        case TokenType::LESS_THAN:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_THAN:
        case TokenType::GREATER_EQUAL:
            if ((leftType == ValueType::NUMBER || leftType == ValueType::UNKNOWN) && 
                (rightType == ValueType::NUMBER || rightType == ValueType::UNKNOWN)) {
                return ValueType::BOOLEAN;
            }
            throw std::runtime_error("Comparison operations require numeric operands");

        case TokenType::AND:
        case TokenType::OR:
            if ((leftType == ValueType::BOOLEAN || leftType == ValueType::UNKNOWN) && 
                (rightType == ValueType::BOOLEAN || rightType == ValueType::UNKNOWN)) {
                return ValueType::BOOLEAN;
            }
            throw std::runtime_error("Logical operations require boolean operands");

        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

ValueType SemanticAnalyzer::analyzeUnaryExpression(UnaryExpression* expr) {
    ValueType operandType = analyzeExpression(expr->operand.get());

    switch (expr->operator_) {
        case TokenType::MINUS:
            if (operandType == ValueType::NUMBER || operandType == ValueType::UNKNOWN) {
                return ValueType::NUMBER;
            }
            throw std::runtime_error("Unary minus requires numeric operand");

        case TokenType::NOT:
            if (operandType == ValueType::BOOLEAN || operandType == ValueType::UNKNOWN) {
                return ValueType::BOOLEAN;
            }
            throw std::runtime_error("Logical not requires boolean operand");

        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

ValueType SemanticAnalyzer::analyzeCallExpression(CallExpression* expr) {
    if (expr->callee->type != NodeType::IDENTIFIER) {
        throw std::runtime_error("Only identifiers can be called");
    }

    Identifier* callee = static_cast<Identifier*>(expr->callee.get());
    Symbol* symbol = currentScope->lookup(callee->name);

    if (!symbol) {
        throw std::runtime_error("Undefined function: " + callee->name);
    }

    if (!symbol->isFunction) {
        throw std::runtime_error("Cannot call non-function: " + callee->name);
    }

    for (auto& arg : expr->arguments) {
        analyzeExpression(arg.get());
    }

    if (callee->name == "say") return ValueType::VOID;
    if (callee->name == "input") return ValueType::STRING;
    if (callee->name == "str") return ValueType::STRING;
    if (callee->name == "num") return ValueType::NUMBER;
    if (callee->name == "bool") return ValueType::BOOLEAN;
    if (callee->name == "len") return ValueType::NUMBER;
    if (callee->name == "type") return ValueType::STRING;
    if (callee->name == "exists") return ValueType::BOOLEAN;
    if (callee->name == "charAt") return ValueType::STRING;
    if (callee->name == "sbuild_new") return ValueType::BOOLEAN;
    if (callee->name == "sbuild_add") return ValueType::BOOLEAN;
    if (callee->name == "sbuild_get") return ValueType::STRING;
    if (callee->name == "abs" || callee->name == "max" || callee->name == "min" ||
        callee->name == "round" || callee->name == "sqrt" || callee->name == "pow" ||
        callee->name == "random") {
        return ValueType::NUMBER;
    }
    if (callee->name == "upper" || callee->name == "lower" || callee->name == "strip" ||
        callee->name == "join" || callee->name == "replace") {
        return ValueType::STRING;
    }

    return symbol->returnType;
}

ValueType SemanticAnalyzer::analyzeIdentifier(Identifier* expr) {
    Symbol* symbol = currentScope->lookup(expr->name);

    if (!symbol) {
        throw std::runtime_error("Undefined variable: " + expr->name);
    }

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

    if (objectType != ValueType::LIST && objectType != ValueType::UNKNOWN) {
        throw std::runtime_error("Cannot index a non-list type.");
    }

    if (indexType != ValueType::NUMBER && indexType != ValueType::UNKNOWN) {
        throw std::runtime_error("List index must be a number.");
    }

    // We don't know the type of the elements in the list at compile time,
    // so we return UNKNOWN.
    return ValueType::UNKNOWN;
}
ValueType SemanticAnalyzer::analyzeDictLiteral(DictLiteral* expr) {
    for (size_t i = 0; i < expr->keys.size(); ++i) {
        ValueType keyType = analyzeExpression(expr->keys[i].get());
        // It's good practice to ensure dictionary keys are strings.
        if (keyType != ValueType::STRING && keyType != ValueType::UNKNOWN) {
            // We could throw an error here, but for now, we'll allow it
            // and let the VM handle potential runtime errors.
        }
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
    ValueType valueType = analyzeExpression(stmt->value.get());
    ValueType targetType = analyzeExpression(stmt->target.get());

    if (targetType != ValueType::UNKNOWN && valueType != ValueType::UNKNOWN && !isCompatible(targetType, valueType)) {
        // This check is tricky with lists, so we keep it simple.
        // A more advanced type system could handle this better.
    }
}

void SemanticAnalyzer::analyzeFunctionDeclaration(FunctionDeclaration* stmt) {
    currentScope->define(stmt->name, ValueType::FUNCTION, true);
    Symbol* funcSymbol = currentScope->lookup(stmt->name);

    enterScope();

    for (const auto& param : stmt->parameters) {
        currentScope->define(param, ValueType::UNKNOWN);
    }

    bool wasInFunction = inFunction;
    ValueType prevReturnType = currentFunctionReturnType;

    inFunction = true;
    currentFunctionReturnType = ValueType::UNKNOWN;

    for (auto& bodyStmt : stmt->body) {
        analyzeStatement(bodyStmt.get());
    }

    if (funcSymbol) {
        funcSymbol->returnType = currentFunctionReturnType;
    }

    inFunction = wasInFunction;
    currentFunctionReturnType = prevReturnType;

    exitScope();
}

void SemanticAnalyzer::analyzeIfStatement(IfStatement* stmt) {
    ValueType condType = analyzeExpression(stmt->condition.get());

    if (condType != ValueType::BOOLEAN && condType != ValueType::UNKNOWN) {
        throw std::runtime_error("If condition must be boolean");
    }

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
    ValueType condType = analyzeExpression(stmt->condition.get());

    if (condType != ValueType::BOOLEAN && condType != ValueType::UNKNOWN) {
        throw std::runtime_error("While condition must be boolean");
    }

    loopDepth++;
    enterScope();
    for (auto& bodyStmt : stmt->body) {
        analyzeStatement(bodyStmt.get());
    }
    exitScope();
    loopDepth--;
}

void SemanticAnalyzer::analyzeRepeatStatement(RepeatStatement* stmt) {
    ValueType countType = analyzeExpression(stmt->count.get());

    if (countType != ValueType::NUMBER && countType != ValueType::UNKNOWN) {
        throw std::runtime_error("Repeat count must be a number");
    }

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
        throw std::runtime_error("Return statement outside function");
    }

    ValueType returnType = ValueType::VOID;
    if (stmt->value) {
        returnType = analyzeExpression(stmt->value.get());
    }

    if (currentFunctionReturnType == ValueType::UNKNOWN) {
        currentFunctionReturnType = returnType;
    } else if (currentFunctionReturnType != returnType && returnType != ValueType::VOID) {
        // Allow multiple return paths as long as they are compatible
    }
}

void SemanticAnalyzer::analyzeBreakStatement(BreakStatement* stmt) {
    if (loopDepth == 0) {
        throw std::runtime_error("'break' statement outside of a loop.");
    }
}

void SemanticAnalyzer::analyzeContinueStatement(ContinueStatement* stmt) {
    if (loopDepth == 0) {
        throw std::runtime_error("'continue' statement outside of a loop.");
    }
}

void SemanticAnalyzer::analyzeExpressionStatement(ExpressionStatement* stmt) {
    analyzeExpression(stmt->expression.get());
}

bool SemanticAnalyzer::isCompatible(ValueType expected, ValueType actual) {
    if (expected == ValueType::UNKNOWN || actual == ValueType::UNKNOWN) {
        return true;
    }
    return expected == actual;
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
        case ValueType::UNKNOWN: return "unknown";
        default: return "unknown";
    }
}
