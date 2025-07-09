#include <iostream>
#include <cassert>
#include <memory>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/semantic.h"

void testVariableDeclaration() {
    std::cout << "Testing variable declaration semantic analysis..." << std::endl;
    
    Lexer lexer("let x = 42");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ Variable declaration semantic test passed" << std::endl;
}

void testUndefinedVariable() {
    std::cout << "Testing undefined variable error..." << std::endl;
    
    Lexer lexer("let x = y");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(ast.get());
        assert(false && "Should have thrown an error for undefined variable");
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("Undefined variable") != std::string::npos);
    }
    
    std::cout << "✓ Undefined variable error test passed" << std::endl;
}

void testFunctionDeclaration() {
    std::cout << "Testing function declaration semantic analysis..." << std::endl;
    
    Lexer lexer("makef add(a, b):\n    return a + b\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ Function declaration semantic test passed" << std::endl;
}

void testFunctionCall() {
    std::cout << "Testing function call semantic analysis..." << std::endl;
    
    Lexer lexer("makef add(a, b):\n    return a + b\nend\nlet result = add(5, 3)");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ Function call semantic test passed" << std::endl;
}

void testUndefinedFunction() {
    std::cout << "Testing undefined function error..." << std::endl;
    
    Lexer lexer("let result = unknown_func(5)");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(ast.get());
        assert(false && "Should have thrown an error for undefined function");
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("Undefined function") != std::string::npos);
    }
    
    std::cout << "✓ Undefined function error test passed" << std::endl;
}

void testBinaryExpressionTypes() {
    std::cout << "Testing binary expression type checking..." << std::endl;
    
    Lexer lexer("let x = 5 + 3");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ Binary expression type test passed" << std::endl;
}

void testStringConcatenation() {
    std::cout << "Testing string concatenation..." << std::endl;
    
    Lexer lexer("let greeting = \"Hello, \" + \"World!\"");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ String concatenation test passed" << std::endl;
}

void testIfConditionType() {
    std::cout << "Testing if condition type checking..." << std::endl;
    
    Lexer lexer("if 5 > 3:\n    say \"true\"\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ If condition type test passed" << std::endl;
}

void testInvalidIfCondition() {
    std::cout << "Testing invalid if condition error..." << std::endl;
    
    Lexer lexer("if \"hello\":\n    say \"world\"\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(ast.get());
        assert(false && "Should have thrown an error for non-boolean condition");
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("condition must be boolean") != std::string::npos);
    }
    
    std::cout << "✓ Invalid if condition error test passed" << std::endl;
}

void testWhileConditionType() {
    std::cout << "Testing while condition type checking..." << std::endl;
    
    Lexer lexer("while x < 10:\n    let x = x + 1\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(ast.get());
        assert(false && "Should have thrown an error for undefined variable");
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("Undefined variable") != std::string::npos);
    }
    
    std::cout << "✓ While condition type test passed" << std::endl;
}

void testReturnOutsideFunction() {
    std::cout << "Testing return outside function error..." << std::endl;
    
    Lexer lexer("return 5");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(ast.get());
        assert(false && "Should have thrown an error for return outside function");
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        assert(error.find("Return statement outside function") != std::string::npos);
    }
    
    std::cout << "✓ Return outside function error test passed" << std::endl;
}

void testBuiltinFunctions() {
    std::cout << "Testing built-in functions..." << std::endl;
    
    Lexer lexer("say \"Hello\"\nlet s = str(42)\nlet n = num(\"3.14\")");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ Built-in functions test passed" << std::endl;
}

void testScopeResolution() {
    std::cout << "Testing scope resolution..." << std::endl;
    
    Lexer lexer("let x = 5\nmakef test():\n    let x = 10\n    return x\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast.get());
    
    std::cout << "✓ Scope resolution test passed" << std::endl;
}

int main() {
    std::cout << "Running Semantic Analysis Tests..." << std::endl;
    
    try {
        testVariableDeclaration();
        testUndefinedVariable();
        testFunctionDeclaration();
        testFunctionCall();
        testUndefinedFunction();
        testBinaryExpressionTypes();
        testStringConcatenation();
        testIfConditionType();
        testInvalidIfCondition();
        testWhileConditionType();
        testReturnOutsideFunction();
        testBuiltinFunctions();
        testScopeResolution();
        
        std::cout << "\n✅ All semantic analysis tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
