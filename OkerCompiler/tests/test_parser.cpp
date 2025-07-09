#include <iostream>
#include <cassert>
#include <memory>
#include "../src/lexer.h"
#include "../src/parser.h"

void testVariableDeclaration() {
    std::cout << "Testing variable declaration..." << std::endl;
    
    Lexer lexer("let x = 42");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    assert(ast->statements[0]->type == NodeType::VARIABLE_DECLARATION);
    
    VariableDeclaration* varDecl = static_cast<VariableDeclaration*>(ast->statements[0].get());
    assert(varDecl->name == "x");
    assert(varDecl->initializer->type == NodeType::NUMBER_LITERAL);
    
    std::cout << "✓ Variable declaration test passed" << std::endl;
}

void testFunctionDeclaration() {
    std::cout << "Testing function declaration..." << std::endl;
    
    Lexer lexer("makef add(a, b):\n    return a + b\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    assert(ast->statements[0]->type == NodeType::FUNCTION_DECLARATION);
    
    FunctionDeclaration* funcDecl = static_cast<FunctionDeclaration*>(ast->statements[0].get());
    assert(funcDecl->name == "add");
    assert(funcDecl->parameters.size() == 2);
    assert(funcDecl->parameters[0] == "a");
    assert(funcDecl->parameters[1] == "b");
    assert(funcDecl->body.size() == 1);
    assert(funcDecl->body[0]->type == NodeType::RETURN_STATEMENT);
    
    std::cout << "✓ Function declaration test passed" << std::endl;
}

void testIfStatement() {
    std::cout << "Testing if statement..." << std::endl;
    
    Lexer lexer("if x > 5:\n    say \"big\"\nelse:\n    say \"small\"\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    assert(ast->statements[0]->type == NodeType::IF_STATEMENT);
    
    IfStatement* ifStmt = static_cast<IfStatement*>(ast->statements[0].get());
    assert(ifStmt->condition->type == NodeType::BINARY_EXPRESSION);
    assert(ifStmt->thenBranch.size() == 1);
    assert(ifStmt->elseBranch.size() == 1);
    
    std::cout << "✓ If statement test passed" << std::endl;
}

void testWhileStatement() {
    std::cout << "Testing while statement..." << std::endl;
    
    Lexer lexer("while x < 10:\n    let x = x + 1\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    assert(ast->statements[0]->type == NodeType::WHILE_STATEMENT);
    
    WhileStatement* whileStmt = static_cast<WhileStatement*>(ast->statements[0].get());
    assert(whileStmt->condition->type == NodeType::BINARY_EXPRESSION);
    assert(whileStmt->body.size() == 1);
    
    std::cout << "✓ While statement test passed" << std::endl;
}

void testRepeatStatement() {
    std::cout << "Testing repeat statement..." << std::endl;
    
    Lexer lexer("repeat 5:\n    say \"hello\"\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    assert(ast->statements[0]->type == NodeType::REPEAT_STATEMENT);
    
    RepeatStatement* repeatStmt = static_cast<RepeatStatement*>(ast->statements[0].get());
    assert(repeatStmt->count->type == NodeType::NUMBER_LITERAL);
    assert(repeatStmt->body.size() == 1);
    
    std::cout << "✓ Repeat statement test passed" << std::endl;
}

void testBinaryExpression() {
    std::cout << "Testing binary expression..." << std::endl;
    
    Lexer lexer("let result = a + b * c");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    VariableDeclaration* varDecl = static_cast<VariableDeclaration*>(ast->statements[0].get());
    assert(varDecl->initializer->type == NodeType::BINARY_EXPRESSION);
    
    BinaryExpression* expr = static_cast<BinaryExpression*>(varDecl->initializer.get());
    assert(expr->operator_ == TokenType::PLUS);
    assert(expr->left->type == NodeType::IDENTIFIER);
    assert(expr->right->type == NodeType::BINARY_EXPRESSION);
    
    std::cout << "✓ Binary expression test passed" << std::endl;
}

void testFunctionCall() {
    std::cout << "Testing function call..." << std::endl;
    
    Lexer lexer("let result = add(5, 3)");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    VariableDeclaration* varDecl = static_cast<VariableDeclaration*>(ast->statements[0].get());
    assert(varDecl->initializer->type == NodeType::CALL_EXPRESSION);
    
    CallExpression* call = static_cast<CallExpression*>(varDecl->initializer.get());
    assert(call->callee->type == NodeType::IDENTIFIER);
    assert(call->arguments.size() == 2);
    
    std::cout << "✓ Function call test passed" << std::endl;
}

void testAssignment() {
    std::cout << "Testing assignment..." << std::endl;
    
    Lexer lexer("x = 10");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    assert(ast->statements[0]->type == NodeType::ASSIGNMENT);
    
    Assignment* assignment = static_cast<Assignment*>(ast->statements[0].get());
    assert(assignment->target->type == NodeType::IDENTIFIER);
    assert(assignment->value->type == NodeType::NUMBER_LITERAL);
    
    std::cout << "✓ Assignment test passed" << std::endl;
}

void testNestedExpressions() {
    std::cout << "Testing nested expressions..." << std::endl;
    
    Lexer lexer("let result = (a + b) * (c - d)");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    VariableDeclaration* varDecl = static_cast<VariableDeclaration*>(ast->statements[0].get());
    assert(varDecl->initializer->type == NodeType::BINARY_EXPRESSION);
    
    BinaryExpression* expr = static_cast<BinaryExpression*>(varDecl->initializer.get());
    assert(expr->operator_ == TokenType::MULTIPLY);
    assert(expr->left->type == NodeType::BINARY_EXPRESSION);
    assert(expr->right->type == NodeType::BINARY_EXPRESSION);
    
    std::cout << "✓ Nested expressions test passed" << std::endl;
}

void testUnaryExpression() {
    std::cout << "Testing unary expression..." << std::endl;
    
    Lexer lexer("let result = -x");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast->statements.size() == 1);
    VariableDeclaration* varDecl = static_cast<VariableDeclaration*>(ast->statements[0].get());
    assert(varDecl->initializer->type == NodeType::UNARY_EXPRESSION);
    
    UnaryExpression* expr = static_cast<UnaryExpression*>(varDecl->initializer.get());
    assert(expr->operator_ == TokenType::MINUS);
    assert(expr->operand->type == NodeType::IDENTIFIER);
    
    std::cout << "✓ Unary expression test passed" << std::endl;
}

int main() {
    std::cout << "Running Parser Tests..." << std::endl;
    
    try {
        testVariableDeclaration();
        testFunctionDeclaration();
        testIfStatement();
        testWhileStatement();
        testRepeatStatement();
        testBinaryExpression();
        testFunctionCall();
        testAssignment();
        testNestedExpressions();
        testUnaryExpression();
        
        std::cout << "\n✅ All parser tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
