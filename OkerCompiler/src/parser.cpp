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
        if (elem) elem->print(indentLevel + 2);
    }
}

void DictLiteral::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "DictLiteral:\n";
    for (size_t i = 0; i < keys.size(); ++i) {
        std::cout << indent(indentLevel + 1) << "Pair " << i << ":\n";
        std::cout << indent(indentLevel + 2) << "Key:\n";
        if (keys[i]) keys[i]->print(indentLevel + 3);
        std::cout << indent(indentLevel + 2) << "Value:\n";
        if (values[i]) values[i]->print(indentLevel + 3);
    }
}

void IndexExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "IndexExpression:\n";
    std::cout << indent(indentLevel + 1) << "Object:\n";
    if (object) object->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Index:\n";
    if (index) index->print(indentLevel + 2);
}

void BinaryExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "BinaryExpression:\n";
    std::cout << indent(indentLevel + 1) << "Left:\n";
    if (left) left->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Operator: " << static_cast<int>(operator_) << "\n";
    std::cout << indent(indentLevel + 1) << "Right:\n";
    if (right) right->print(indentLevel + 2);
}

void UnaryExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "UnaryExpression:\n";
    std::cout << indent(indentLevel + 1) << "Operator: " << static_cast<int>(operator_) << "\n";
    std::cout << indent(indentLevel + 1) << "Operand:\n";
    if (operand) operand->print(indentLevel + 2);
}

void CallExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "CallExpression" << (isNew ? " (new)" : "") << ":\n";
    std::cout << indent(indentLevel + 1) << "Callee:\n";
    if (callee) callee->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Arguments:\n";
    for (const auto& arg : arguments) {
        if (arg) arg->print(indentLevel + 2);
    }
}

void MemberExpression::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "MemberExpression:\n";
    std::cout << indent(indentLevel + 1) << "Object:\n";
    if (object) object->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Property:\n";
    if (property) property->print(indentLevel + 2);
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
    if (target) target->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Value:\n";
    if (value) value->print(indentLevel + 2);
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

void ClassDeclaration::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "ClassDeclaration: " << name << "\n";
    std::cout << indent(indentLevel + 1) << "Methods:\n";
    for (const auto& method : methods) {
        if (method) method->print(indentLevel + 2);
    }
}


void IfStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "IfStatement:\n";
    std::cout << indent(indentLevel + 1) << "Condition:\n";
    if (condition) condition->print(indentLevel + 2);
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
    if (condition) condition->print(indentLevel + 2);
    std::cout << indent(indentLevel + 1) << "Body:\n";
    for (const auto& stmt : body) {
        if(stmt) stmt->print(indentLevel + 2);
    }
}

void RepeatStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "RepeatStatement:\n";
    std::cout << indent(indentLevel + 1) << "Count:\n";
    if (count) count->print(indentLevel + 2);
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
    if (expression) expression->print(indentLevel + 1);
}

void BreakStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "BreakStatement\n";
}

void ContinueStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "ContinueStatement\n";
}

void TryStatement::print(int indentLevel) const {
    std::cout << indent(indentLevel) << "TryStatement:\n";
    std::cout << indent(indentLevel + 1) << "Try Block:\n";
    for (const auto& stmt : tryBlock) {
        if(stmt) stmt->print(indentLevel + 2);
    }
    std::cout << indent(indentLevel + 1) << "Fail Block:\n";
    for (const auto& stmt : failBlock) {
        if(stmt) stmt->print(indentLevel + 2);
    }
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
    while (match(TokenType::NEWLINE) || match(TokenType::COMMENT)) {}
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    skipNewlines();
    while (!isAtEnd()) {
        program->statements.push_back(statement());
        skipNewlines();
    }
    return program;
}

std::unique_ptr<Statement> Parser::statement() {
    if (check(TokenType::CLASS)) return classDeclaration();
    if (check(TokenType::LET)) return letStatement();
    if (check(TokenType::SAY)) return sayStatement();
    if (check(TokenType::IF)) return ifStatement();
    if (check(TokenType::WHILE)) return whileStatement();
    if (check(TokenType::REPEAT)) return repeatStatement();
    if (check(TokenType::MAKEF)) return functionDeclaration();
    if (check(TokenType::RETURN)) return returnStatement();
    if (check(TokenType::BREAK)) return breakStatement();
    if (check(TokenType::CONTINUE)) return continueStatement();
    if (check(TokenType::TRY)) return tryStatement();

    auto expr = expression();
    if (match(TokenType::ASSIGN)) {
        auto value = expression();
        return std::make_unique<Assignment>(std::move(expr), std::move(value));
    }

    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::classDeclaration() {
    advance(); // consume 'class'
    if (!check(TokenType::IDENTIFIER)) throw std::runtime_error(format_error("Expected class name", peek()));
    std::string name = advance().value;

    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after class name", peek()));
    skipNewlines();

    std::vector<std::unique_ptr<FunctionDeclaration>> methods;
    while (!check(TokenType::END) && !isAtEnd()) {
        if (!check(TokenType::MAKEF)) throw std::runtime_error(format_error("Only methods (makef) are allowed inside a class", peek()));
        methods.push_back(std::unique_ptr<FunctionDeclaration>(static_cast<FunctionDeclaration*>(functionDeclaration().release())));
        skipNewlines();
    }

    if (!match(TokenType::END)) throw std::runtime_error(format_error("Expected 'end' to close class", peek()));
    return std::make_unique<ClassDeclaration>(name, std::move(methods));
}

std::unique_ptr<Statement> Parser::letStatement() {
    advance(); // consume 'let'
    if (!check(TokenType::IDENTIFIER)) throw std::runtime_error(format_error("Expected identifier after 'let'", peek()));
    std::string name = advance().value;
    if (!match(TokenType::ASSIGN)) throw std::runtime_error(format_error("Expected '=' after variable name", peek()));
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
    Token ifTok = advance();
    auto condition = expression();
    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after if condition", peek()));
    skipNewlines();
    auto ifStmt = std::make_unique<IfStatement>(std::move(condition));
    while (!check(TokenType::ELSE) && !check(TokenType::END) && !isAtEnd()) {
        ifStmt->thenBranch.push_back(statement());
        skipNewlines();
    }
    if (match(TokenType::ELSE)) {
        if (match(TokenType::COLON)) {
            skipNewlines();
            while (!check(TokenType::END) && !isAtEnd()) {
                ifStmt->elseBranch.push_back(statement());
                skipNewlines();
            }
        } else {
            ifStmt->elseBranch.push_back(ifStatement());
        }
    }
    if (!match(TokenType::END)) throw std::runtime_error(format_error("Expected 'end' to close 'if'", peek()));
    return ifStmt;
}

std::unique_ptr<Statement> Parser::whileStatement() {
    advance(); // consume 'while'
    auto condition = expression();
    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after while condition", peek()));
    skipNewlines();
    auto whileStmt = std::make_unique<WhileStatement>(std::move(condition));
    while (!check(TokenType::END) && !isAtEnd()) {
        whileStmt->body.push_back(statement());
        skipNewlines();
    }
    if (!match(TokenType::END)) throw std::runtime_error(format_error("Expected 'end' to close 'while'", peek()));
    return whileStmt;
}
std::unique_ptr<Statement> Parser::repeatStatement() {
    advance(); // consume 'repeat'
    auto count = expression();
    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after repeat count", peek()));
    skipNewlines();
    auto repeatStmt = std::make_unique<RepeatStatement>(std::move(count));
    while (!check(TokenType::END) && !isAtEnd()) {
        repeatStmt->body.push_back(statement());
        skipNewlines();
    }
    if (!match(TokenType::END)) throw std::runtime_error(format_error("Expected 'end' to close 'repeat'", peek()));
    return repeatStmt;
}

std::unique_ptr<Statement> Parser::tryStatement() {
    advance(); // consume 'try'
    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after 'try'", peek()));
    skipNewlines();
    std::vector<std::unique_ptr<Statement>> tryBlock;
    while (!check(TokenType::FAIL) && !isAtEnd()) {
        tryBlock.push_back(statement());
        skipNewlines();
    }
    if (!match(TokenType::FAIL)) throw std::runtime_error(format_error("Expected 'fail' block", peek()));
    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after 'fail'", peek()));
    skipNewlines();
    std::vector<std::unique_ptr<Statement>> failBlock;
    while (!check(TokenType::END) && !isAtEnd()) {
        failBlock.push_back(statement());
        skipNewlines();
    }
    if (!match(TokenType::END)) throw std::runtime_error(format_error("Expected 'end' to close 'try/fail'", peek()));
    return std::make_unique<TryStatement>(std::move(tryBlock), std::move(failBlock));
}

std::unique_ptr<Statement> Parser::functionDeclaration() {
    advance(); // consume 'makef'
    if (!check(TokenType::IDENTIFIER)) throw std::runtime_error(format_error("Expected function name", peek()));
    std::string name = advance().value;
    if (!match(TokenType::LPAREN)) throw std::runtime_error(format_error("Expected '(' after function name", peek()));
    auto funcDecl = std::make_unique<FunctionDeclaration>(name);
    if (!check(TokenType::RPAREN)) {
        do {
            if (!check(TokenType::IDENTIFIER)) throw std::runtime_error(format_error("Expected parameter name", peek()));
            funcDecl->parameters.push_back(advance().value);
        } while (match(TokenType::COMMA));
    }
    if (!match(TokenType::RPAREN)) throw std::runtime_error(format_error("Expected ')' after parameters", peek()));
    if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after function signature", peek()));
    skipNewlines();
    while (!check(TokenType::END) && !isAtEnd()) {
        funcDecl->body.push_back(statement());
        skipNewlines();
    }
    if (!match(TokenType::END)) throw std::runtime_error(format_error("Expected 'end' to close function", peek()));
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
    Token tok = advance();
    return std::make_unique<BreakStatement>(tok.line, tok.column);
}
std::unique_ptr<Statement> Parser::continueStatement() {
    Token tok = advance();
    return std::make_unique<ContinueStatement>(tok.line, tok.column);
}
std::unique_ptr<Expression> Parser::expression() {
    return logicalOr();
}
std::unique_ptr<Expression> Parser::logicalOr() {
    auto expr = logicalAnd();
    while (match(TokenType::OR)) {
        expr = std::make_unique<BinaryExpression>(std::move(expr), TokenType::OR, logicalAnd());
    }
    return expr;
}
std::unique_ptr<Expression> Parser::logicalAnd() {
    auto expr = equality();
    while (match(TokenType::AND)) {
        expr = std::make_unique<BinaryExpression>(std::move(expr), TokenType::AND, equality());
    }
    return expr;
}
std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
        TokenType op = tokens[current - 1].type;
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, comparison());
    }
    return expr;
}
std::unique_ptr<Expression> Parser::comparison() {
    auto expr = addition();
    while (match(TokenType::GREATER_THAN) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS_THAN) || match(TokenType::LESS_EQUAL)) {
        TokenType op = tokens[current - 1].type;
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, addition());
    }
    return expr;
}
std::unique_ptr<Expression> Parser::addition() {
    auto expr = multiplication();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        TokenType op = tokens[current - 1].type;
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, multiplication());
    }
    return expr;
}
std::unique_ptr<Expression> Parser::multiplication() {
    auto expr = unary();
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        TokenType op = tokens[current - 1].type;
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, unary());
    }
    return expr;
}
std::unique_ptr<Expression> Parser::unary() {
    if (match(TokenType::NOT) || match(TokenType::MINUS)) {
        TokenType op = tokens[current - 1].type;
        return std::make_unique<UnaryExpression>(op, unary());
    }
    return call();
}
std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();
    while (true) {
        if (match(TokenType::LPAREN)) {
            auto callExpr = std::make_unique<CallExpression>(std::move(expr));
            if (!check(TokenType::RPAREN)) {
                do {
                    callExpr->arguments.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            if (!match(TokenType::RPAREN)) throw std::runtime_error(format_error("Expected ')' after arguments", peek()));
            expr = std::move(callExpr);
        } else if (match(TokenType::LBRACKET)) {
            auto index = expression();
            if (!match(TokenType::RBRACKET)) throw std::runtime_error(format_error("Expected ']' after index", peek()));
            expr = std::make_unique<IndexExpression>(std::move(expr), std::move(index));
        } else if (match(TokenType::DOT)) {
            if (!check(TokenType::IDENTIFIER)) throw std::runtime_error(format_error("Expected property name after '.'", peek()));
            auto property = std::make_unique<Identifier>(advance().value);
            expr = std::make_unique<MemberExpression>(std::move(expr), std::move(property));
        }
        else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::BOOLEAN)) return std::make_unique<BooleanLiteral>(tokens[current - 1].value == "true");
    if (match(TokenType::NUMBER)) return std::make_unique<NumberLiteral>(std::stod(tokens[current - 1].value));
    if (match(TokenType::STRING)) return std::make_unique<StringLiteral>(tokens[current - 1].value);

    // CORRECTED 'new' EXPRESSION LOGIC
    if (match(TokenType::NEW)) {
        auto expr = call(); // Parses the `Person(...)` part
        if (expr->type != NodeType::CALL_EXPRESSION) {
            throw std::runtime_error(format_error("Expected constructor call after 'new'", peek()));
        }
        // Mark this CallExpression as a 'new' call
        static_cast<CallExpression*>(expr.get())->isNew = true;
        return expr;
    }

    if (match(TokenType::THIS)) {
        return std::make_unique<Identifier>("this");
    }

    if (match(TokenType::IDENTIFIER)) return std::make_unique<Identifier>(tokens[current - 1].value);

    if (match(TokenType::LPAREN)) {
        auto expr = expression();
        if (!match(TokenType::RPAREN)) throw std::runtime_error(format_error("Expected ')' after expression", peek()));
        return expr;
    }

    if (match(TokenType::LBRACKET)) {
        std::vector<std::unique_ptr<Expression>> elements;
        skipNewlines();
        if (!check(TokenType::RBRACKET)) {
            do {
                skipNewlines();
                if (check(TokenType::RBRACKET)) break;
                elements.push_back(expression());
                skipNewlines();
            } while (match(TokenType::COMMA));
        }
        if (!match(TokenType::RBRACKET)) throw std::runtime_error(format_error("Expected ']' after list elements", peek()));
        return std::make_unique<ListLiteral>(std::move(elements));
    }

    if (match(TokenType::LBRACE)) {
        std::vector<std::unique_ptr<Expression>> keys;
        std::vector<std::unique_ptr<Expression>> values;
        skipNewlines();
        if (!check(TokenType::RBRACE)) {
            do {
                skipNewlines();
                if (check(TokenType::RBRACE)) break;
                keys.push_back(primary());
                if (!match(TokenType::COLON)) throw std::runtime_error(format_error("Expected ':' after dictionary key", peek()));
                values.push_back(expression());
                skipNewlines();
            } while (match(TokenType::COMMA));
        }
        if (!match(TokenType::RBRACE)) throw std::runtime_error(format_error("Expected '}' to close dictionary", peek()));
        return std::make_unique<DictLiteral>(std::move(keys), std::move(values));
    }

    throw std::runtime_error(format_error("Expected expression", peek()));
}