#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <string>

// Utility function for indentation
std::string indent(int level) {
    return std::string(level * 2, ' ');
}

// Helper function to create formatted error messages
std::string format_error(const std::string& message, const Token& token) {
    return "Line " + std::to_string(token.line) + ":" + std::to_string(token.column) + ": " + message;
}

// Program implementation
void Program::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "Program:\n";
    for (const auto& stmt : statements) {
        if(stmt) stmt->print(indentLevel + 1);
    }
}

// Expression implementations
void Identifier::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "Identifier: " << name << "\n";
}

void NumberLiteral::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "Number: " << value << "\n";
}

void StringLiteral::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "String: \"" << value << "\"\n";
}

void BooleanLiteral::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "Boolean: " << (value ? "true" : "false") << "\n";
}

void ListLiteral::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "ListLiteral:\n";
    std::cout << indent(indentLevel + 1) << "Elements:\n";
    for (const auto& elem : elements) {
        elem->print(indentLevel + 2);
    }
}

void IndexExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "IndexExpression:\n";
    std::cout << indent(indentLevel + 1) << "Object:\n";
    object->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Index:\n";
    index->print(indentLevel + 2);
}

void BinaryExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "BinaryExpression:\n";
    std::cout << indent(indentLevel + 1) << "Left:\n";
    left->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Operator: " << static_cast<int>(operator_) << "\n";
    std::cout << indent(indentLevel + 1) << "Right:\n";
    right->print(indentLevel + 2);
}

void UnaryExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "UnaryExpression:\n";
    std::cout << indent(indentLevel + 1) << "Operator: " << static_cast<int>(operator_) << "\n";
    std::cout << indent(indentLevel + 1) << "Operand:\n";
    operand->print(indentLevel + 2);
}

void CallExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "CallExpression:\n";
    std::cout << indent(indentLevel + 1) << "Callee:\n";
    callee->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Arguments:\n";
    for (const auto& arg : arguments) {
        arg->print(indentLevel + 2);
    }
}

// Statement implementations
void VariableDeclaration::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "VariableDeclaration: " << name << "\n";
    if (initializer) {
        std::cout << indent(indentLevel + 1) << "Initializer:\n";
        initializer->print(indentLevel + 2);
    }
}

void Assignment::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "Assignment:\n";
    std::cout << indent(indentLevel + 1) << "Target:\n";
    target->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Value:\n";
    value->print(indentLevel + 2);
}

void FunctionDeclaration::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "FunctionDeclaration: " << name << "\n";
    std::cout << indent(indentLevel + 1) << "Parameters:\n";
    for (const auto& param : parameters) {
        std::cout << indent(indentLevel + 2) << param << "\n";
    }
    std::cout << indent(indentLevel + 1) << "Body:\n";
    for (const auto& stmt : body) {
        if(stmt) stmt->print(indentLevel + 2);
    }
}

void IfStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "IfStatement:\n";
    std::cout << indent(indentLevel + 1) << "Condition:\n";
    condition->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Then:\n";
    for (const auto& stmt : thenBranch) {
        if(stmt) stmt->print(indentLevel + 2);
    }
    if (!elseBranch.empty()) {
        std::cout << indent(indentLevel + 1) << "Else:\n";
        for (const auto& stmt : elseBranch) {
            if(stmt) stmt->print(indentLevel + 2);
        }
    }
}

void WhileStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "WhileStatement:\n";
    std::cout << indent(indentLevel + 1) << "Condition:\n";
    condition->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Body:\n";
    for (const auto& stmt : body) {
        if(stmt) stmt->print(indentLevel + 2);
    }
}

void RepeatStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "RepeatStatement:\n";
    std::cout << indent(indentLevel + 1) << "Count:\n";
    count->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Body:\n";
    for (const auto& stmt : body) {
        if(stmt) stmt->print(indentLevel + 2);
    }
}

void ReturnStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "ReturnStatement:\n";
    if (value) {
        std::cout << indent(indentLevel + 1) << "Value:\n";
        value->print(indentLevel + 2);
    }
}

void ExpressionStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "ExpressionStatement:\n";
    expression->print(indentLevel + 1);
}

void BreakStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "BreakStatement\n";
}

void ContinueStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "ContinueStatement\n";
}


// Parser implementation
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

Token& Parser::peek() {
    return tokens[current];
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE) || match(TokenType::COMMENT)) {
        // Skip newlines and comments
    }
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    skipNewlines();

    while (!isAtEnd()) {
        auto stmt = statement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        skipNewlines();
    }

    return program;
}

std::unique_ptr<Statement> Parser::statement() {
    if (check(TokenType::LET)) return letStatement();
    if (check(TokenType::SAY)) return sayStatement();
    if (check(TokenType::IF)) return ifStatement();
    if (check(TokenType::WHILE)) return whileStatement();
    if (check(TokenType::REPEAT)) return repeatStatement();
    if (check(TokenType::MAKEF)) return functionDeclaration();
    if (check(TokenType::RETURN)) return returnStatement();
    if (check(TokenType::BREAK)) return breakStatement();
    if (check(TokenType::CONTINUE)) return continueStatement();

    if (check(TokenType::IDENTIFIER)) {
        size_t savedPos = current;
        advance();
        // Check for both assignment and index assignment
        if (match(TokenType::ASSIGN) || check(TokenType::LBRACKET)) {
            current = savedPos;
            return assignmentStatement();
        }
        current = savedPos;
    }

    return expressionStatement();
}

std::unique_ptr<Statement> Parser::letStatement() {
    Token letTok = advance(); // consume 'let'

    if (!check(TokenType::IDENTIFIER)) {
        throw std::runtime_error(format_error("Expected identifier after 'let'", letTok));
    }

    std::string name = advance().value;

    if (!match(TokenType::ASSIGN)) {
        throw std::runtime_error(format_error("Expected '=' after variable name", peek()));
    }

    auto initializer = expression();

    return std::make_unique<VariableDeclaration>(name, std::move(initializer));
}

std::unique_ptr<Statement> Parser::sayStatement() {
    advance(); // consume 'say'

    auto expr = expression();

    auto callee = std::make_unique<Identifier>("say");
    auto call = std::make_unique<CallExpression>(std::move(callee));
    call->arguments.push_back(std::move(expr));

    return std::make_unique<ExpressionStatement>(std::move(call));
}

std::unique_ptr<Statement> Parser::ifStatement() {
    Token ifTok = advance(); // consume 'if'

    auto condition = expression();

    if (!match(TokenType::COLON)) {
        throw std::runtime_error(format_error("Expected ':' after if condition", peek()));
    }

    skipNewlines();

    auto ifStmt = std::make_unique<IfStatement>(std::move(condition));

    while (!check(TokenType::ELSE) && !check(TokenType::END) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::ELSE) || check(TokenType::END) || isAtEnd()) break;

        auto stmt = statement();
        if (stmt) {
            ifStmt->thenBranch.push_back(std::move(stmt));
        }
    }

    if (match(TokenType::ELSE)) {
        if (!match(TokenType::COLON)) {
            throw std::runtime_error(format_error("Expected ':' after else", peek()));
        }
        skipNewlines();

        while (!check(TokenType::END) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::END) || isAtEnd()) break;

            auto stmt = statement();
            if (stmt) {
                ifStmt->elseBranch.push_back(std::move(stmt));
            }
        }
    }

    if (!match(TokenType::END)) {
        throw std::runtime_error(format_error("Expected 'end' to close 'if' statement starting on line " + std::to_string(ifTok.line), peek()));
    }

    return ifStmt;
}

std::unique_ptr<Statement> Parser::whileStatement() {
    Token whileTok = advance(); // consume 'while'

    auto condition = expression();

    if (!match(TokenType::COLON)) {
        throw std::runtime_error(format_error("Expected ':' after while condition", peek()));
    }

    skipNewlines();

    auto whileStmt = std::make_unique<WhileStatement>(std::move(condition));

    while (!check(TokenType::END) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::END) || isAtEnd()) break;

        auto stmt = statement();
        if (stmt) {
            whileStmt->body.push_back(std::move(stmt));
        }
    }

    if (!match(TokenType::END)) {
        throw std::runtime_error(format_error("Expected 'end' to close 'while' statement starting on line " + std::to_string(whileTok.line), peek()));
    }

    return whileStmt;
}

std::unique_ptr<Statement> Parser::repeatStatement() {
    Token repeatTok = advance(); // consume 'repeat'

    auto count = expression();

    if (!match(TokenType::COLON)) {
        throw std::runtime_error(format_error("Expected ':' after repeat count", peek()));
    }

    skipNewlines();

    auto repeatStmt = std::make_unique<RepeatStatement>(std::move(count));

    while (!check(TokenType::END) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::END) || isAtEnd()) break;

        auto stmt = statement();
        if (stmt) {
            repeatStmt->body.push_back(std::move(stmt));
        }
    }

    if (!match(TokenType::END)) {
        throw std::runtime_error(format_error("Expected 'end' to close 'repeat' statement starting on line " + std::to_string(repeatTok.line), peek()));
    }

    return repeatStmt;
}

std::unique_ptr<Statement> Parser::functionDeclaration() {
    Token makefTok = advance(); // consume 'makef'

    if (!check(TokenType::IDENTIFIER)) {
        throw std::runtime_error(format_error("Expected function name after 'makef'", peek()));
    }

    std::string name = advance().value;

    if (!match(TokenType::LPAREN)) {
        throw std::runtime_error(format_error("Expected '(' after function name", peek()));
    }

    auto funcDecl = std::make_unique<FunctionDeclaration>(name);

    if (!check(TokenType::RPAREN)) {
        do {
            if (!check(TokenType::IDENTIFIER)) {
                throw std::runtime_error(format_error("Expected parameter name", peek()));
            }
            funcDecl->parameters.push_back(advance().value);
        } while (match(TokenType::COMMA));
    }

    if (!match(TokenType::RPAREN)) {
        throw std::runtime_error(format_error("Expected ')' after parameters", peek()));
    }

    if (!match(TokenType::COLON)) {
        throw std::runtime_error(format_error("Expected ':' after function signature", peek()));
    }

    skipNewlines();

    while (!check(TokenType::END) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::END) || isAtEnd()) break;

        auto stmt = statement();
        if (stmt) {
            funcDecl->body.push_back(std::move(stmt));
        }
    }

    if (!match(TokenType::END)) {
        throw std::runtime_error(format_error("Expected 'end' to close 'makef' statement starting on line " + std::to_string(makefTok.line), peek()));
    }

    return funcDecl;
}

std::unique_ptr<Statement> Parser::returnStatement() {
    advance(); // consume 'return'

    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::NEWLINE) && !check(TokenType::END) && !isAtEnd()) {
        value = expression();
    }

    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Statement> Parser::breakStatement() {
    Token tok = advance(); // consume 'break'
    return std::make_unique<BreakStatement>(tok.line, tok.column);
}

std::unique_ptr<Statement> Parser::continueStatement() {
    Token tok = advance(); // consume 'continue'
    return std::make_unique<ContinueStatement>(tok.line, tok.column);
}

std::unique_ptr<Statement> Parser::assignmentStatement() {
    auto target = expression();

    if (!match(TokenType::ASSIGN)) {
        throw std::runtime_error(format_error("Expected '=' in assignment", peek()));
    }

    auto value = expression();

    return std::make_unique<Assignment>(std::move(target), std::move(value));
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr = expression();
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

// Expression parsing methods
std::unique_ptr<Expression> Parser::expression() {
    return logicalOr();
}

std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr = logicalAnd();

    while (match(TokenType::OR)) {
        TokenType op = tokens[current - 1].type;
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr = equality();

    while (match(TokenType::AND)) {
        TokenType op = tokens[current - 1].type;
        auto right = equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();

    while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
        TokenType op = tokens[current - 1].type;
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = addition();

    while (match(TokenType::GREATER_THAN) || match(TokenType::GREATER_EQUAL) ||
           match(TokenType::LESS_THAN) || match(TokenType::LESS_EQUAL)) {
        TokenType op = tokens[current - 1].type;
        auto right = addition();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::addition() {
    auto expr = multiplication();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        TokenType op = tokens[current - 1].type;
        auto right = multiplication();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::multiplication() {
    auto expr = unary();

    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        TokenType op = tokens[current - 1].type;
        auto right = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        TokenType op = tokens[current - 1].type;
        auto right = unary();
        return std::make_unique<UnaryExpression>(op, std::move(right));
    }

    return call();
}

std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();

    while (true) {
        if (match(TokenType::LPAREN)) {
            auto call = std::make_unique<CallExpression>(std::move(expr));

            if (!check(TokenType::RPAREN)) {
                do {
                    call->arguments.push_back(expression());
                } while (match(TokenType::COMMA));
            }

            if (!match(TokenType::RPAREN)) {
                throw std::runtime_error(format_error("Expected ')' after arguments", peek()));
            }

            expr = std::move(call);
        } else if (match(TokenType::LBRACKET)) {
            auto index = expression();
            if (!match(TokenType::RBRACKET)) {
                throw std::runtime_error(format_error("Expected ']' after index", peek()));
            }
            expr = std::make_unique<IndexExpression>(std::move(expr), std::move(index));
        }
        else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::BOOLEAN)) {
        bool value = tokens[current - 1].value == "true";
        return std::make_unique<BooleanLiteral>(value);
    }

    if (match(TokenType::NUMBER)) {
        double value = std::stod(tokens[current - 1].value);
        return std::make_unique<NumberLiteral>(value);
    }

    if (match(TokenType::STRING)) {
        return std::make_unique<StringLiteral>(tokens[current - 1].value);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<Identifier>(tokens[current - 1].value);
    }

    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        if (!match(TokenType::RPAREN)) {
            throw std::runtime_error(format_error("Expected ')' after expression", peek()));
        }
        return expr;
    }

    if (match(TokenType::LBRACKET)) {
        std::vector<std::unique_ptr<Expression>> elements;
        if (!check(TokenType::RBRACKET)) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        if (!match(TokenType::RBRACKET)) {
            throw std::runtime_error(format_error("Expected ']' after list elements", peek()));
        }
        return std::make_unique<ListLiteral>(std::move(elements));
    }

    throw std::runtime_error(format_error("Expected expression", peek()));
}
