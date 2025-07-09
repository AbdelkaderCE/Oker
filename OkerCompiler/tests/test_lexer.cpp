#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../src/lexer.h"

void testBasicTokens() {
    std::cout << "Testing basic tokens..." << std::endl;
    
    Lexer lexer("let x = 42");
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 5); // let, x, =, 42, EOF
    assert(tokens[0].type == TokenType::LET);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].value == "x");
    assert(tokens[2].type == TokenType::ASSIGN);
    assert(tokens[3].type == TokenType::NUMBER);
    assert(tokens[3].value == "42");
    assert(tokens[4].type == TokenType::EOF_TOKEN);
    
    std::cout << "✓ Basic tokens test passed" << std::endl;
}

void testStringLiterals() {
    std::cout << "Testing string literals..." << std::endl;
    
    Lexer lexer("\"Hello, World!\"");
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() == 2); // string, EOF
    assert(tokens[0].type == TokenType::STRING);
    assert(tokens[0].value == "Hello, World!");
    
    std::cout << "✓ String literals test passed" << std::endl;
}

void testKeywords() {
    std::cout << "Testing keywords..." << std::endl;
    
    Lexer lexer("if else while repeat makef return say");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::IF);
    assert(tokens[1].type == TokenType::ELSE);
    assert(tokens[2].type == TokenType::WHILE);
    assert(tokens[3].type == TokenType::REPEAT);
    assert(tokens[4].type == TokenType::MAKEF);
    assert(tokens[5].type == TokenType::RETURN);
    assert(tokens[6].type == TokenType::SAY);
    
    std::cout << "✓ Keywords test passed" << std::endl;
}

void testOperators() {
    std::cout << "Testing operators..." << std::endl;
    
    Lexer lexer("+ - * / % == != < <= > >= and or not");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::PLUS);
    assert(tokens[1].type == TokenType::MINUS);
    assert(tokens[2].type == TokenType::MULTIPLY);
    assert(tokens[3].type == TokenType::DIVIDE);
    assert(tokens[4].type == TokenType::MODULO);
    assert(tokens[5].type == TokenType::EQUAL);
    assert(tokens[6].type == TokenType::NOT_EQUAL);
    assert(tokens[7].type == TokenType::LESS_THAN);
    assert(tokens[8].type == TokenType::LESS_EQUAL);
    assert(tokens[9].type == TokenType::GREATER_THAN);
    assert(tokens[10].type == TokenType::GREATER_EQUAL);
    assert(tokens[11].type == TokenType::AND);
    assert(tokens[12].type == TokenType::OR);
    assert(tokens[13].type == TokenType::NOT);
    
    std::cout << "✓ Operators test passed" << std::endl;
}

void testComments() {
    std::cout << "Testing comments..." << std::endl;
    
    Lexer lexer("~ This is a comment\nlet x = 5");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::COMMENT);
    assert(tokens[1].type == TokenType::NEWLINE);
    assert(tokens[2].type == TokenType::LET);
    
    std::cout << "✓ Comments test passed" << std::endl;
}

void testMultiLineComments() {
    std::cout << "Testing multi-line comments..." << std::endl;
    
    Lexer lexer("~~ This is a\nmulti-line comment ~~\nlet x = 5");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::COMMENT);
    assert(tokens[1].type == TokenType::NEWLINE);
    assert(tokens[2].type == TokenType::LET);
    
    std::cout << "✓ Multi-line comments test passed" << std::endl;
}

void testNumbers() {
    std::cout << "Testing numbers..." << std::endl;
    
    Lexer lexer("42 3.14 0.5");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::NUMBER);
    assert(tokens[0].value == "42");
    assert(tokens[1].type == TokenType::NUMBER);
    assert(tokens[1].value == "3.14");
    assert(tokens[2].type == TokenType::NUMBER);
    assert(tokens[2].value == "0.5");
    
    std::cout << "✓ Numbers test passed" << std::endl;
}

void testBooleans() {
    std::cout << "Testing booleans..." << std::endl;
    
    Lexer lexer("true false");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::BOOLEAN);
    assert(tokens[0].value == "true");
    assert(tokens[1].type == TokenType::BOOLEAN);
    assert(tokens[1].value == "false");
    
    std::cout << "✓ Booleans test passed" << std::endl;
}

void testComplexExpression() {
    std::cout << "Testing complex expression..." << std::endl;
    
    Lexer lexer("if (x > 5 and y < 10):");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::IF);
    assert(tokens[1].type == TokenType::LPAREN);
    assert(tokens[2].type == TokenType::IDENTIFIER);
    assert(tokens[3].type == TokenType::GREATER_THAN);
    assert(tokens[4].type == TokenType::NUMBER);
    assert(tokens[5].type == TokenType::AND);
    assert(tokens[6].type == TokenType::IDENTIFIER);
    assert(tokens[7].type == TokenType::LESS_THAN);
    assert(tokens[8].type == TokenType::NUMBER);
    assert(tokens[9].type == TokenType::RPAREN);
    assert(tokens[10].type == TokenType::COLON);
    
    std::cout << "✓ Complex expression test passed" << std::endl;
}

int main() {
    std::cout << "Running Lexer Tests..." << std::endl;
    
    try {
        testBasicTokens();
        testStringLiterals();
        testKeywords();
        testOperators();
        testComments();
        testMultiLineComments();
        testNumbers();
        testBooleans();
        testComplexExpression();
        
        std::cout << "\n✅ All lexer tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
