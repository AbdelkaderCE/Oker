#include <iostream>
#include <cassert>
#include <memory>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/semantic.h"
#include "../src/codegen.h"

void testVariableDeclaration() {
    std::cout << "Testing variable declaration code generation..." << std::endl;
    
    Lexer lexer("let x = 42");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    assert(bytecode.size() >= 3); // PUSH_NUMBER, DECLARE_VAR, HALT
    assert(bytecode[0].opcode == OpCode::PUSH_NUMBER);
    assert(bytecode[0].operands[0] == "42");
    assert(bytecode[1].opcode == OpCode::DECLARE_VAR);
    assert(bytecode[1].operands[0] == "x");
    
    std::cout << "✓ Variable declaration code generation test passed" << std::endl;
}

void testBinaryExpression() {
    std::cout << "Testing binary expression code generation..." << std::endl;
    
    Lexer lexer("let result = 5 + 3");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    assert(bytecode.size() >= 4); // PUSH_NUMBER, PUSH_NUMBER, ADD, DECLARE_VAR, HALT
    assert(bytecode[0].opcode == OpCode::PUSH_NUMBER);
    assert(bytecode[0].operands[0] == "5");
    assert(bytecode[1].opcode == OpCode::PUSH_NUMBER);
    assert(bytecode[1].operands[0] == "3");
    assert(bytecode[2].opcode == OpCode::ADD);
    assert(bytecode[3].opcode == OpCode::DECLARE_VAR);
    
    std::cout << "✓ Binary expression code generation test passed" << std::endl;
}

void testFunctionDeclaration() {
    std::cout << "Testing function declaration code generation..." << std::endl;
    
    Lexer lexer("makef add(a, b):\n    return a + b\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasDefineFunction = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::DEFINE_FUNCTION) {
            hasDefineFunction = true;
            assert(instr.operands.size() >= 3);
            assert(instr.operands[0] == "add");
            break;
        }
    }
    assert(hasDefineFunction);
    
    std::cout << "✓ Function declaration code generation test passed" << std::endl;
}

void testFunctionCall() {
    std::cout << "Testing function call code generation..." << std::endl;
    
    Lexer lexer("let result = add(5, 3)");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasCall = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::CALL) {
            hasCall = true;
            assert(instr.operands.size() >= 2);
            assert(instr.operands[0] == "add");
            assert(instr.operands[1] == "2");
            break;
        }
    }
    assert(hasCall);
    
    std::cout << "✓ Function call code generation test passed" << std::endl;
}

void testBuiltinCall() {
    std::cout << "Testing built-in call code generation..." << std::endl;
    
    Lexer lexer("say \"Hello, World!\"");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasBuiltinCall = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::BUILTIN_CALL) {
            hasBuiltinCall = true;
            assert(instr.operands.size() >= 2);
            assert(instr.operands[0] == "say");
            assert(instr.operands[1] == "1");
            break;
        }
    }
    assert(hasBuiltinCall);
    
    std::cout << "✓ Built-in call code generation test passed" << std::endl;
}

void testIfStatement() {
    std::cout << "Testing if statement code generation..." << std::endl;
    
    Lexer lexer("if x > 5:\n    say \"big\"\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasJumpIfFalse = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::JUMP_IF_FALSE) {
            hasJumpIfFalse = true;
            break;
        }
    }
    assert(hasJumpIfFalse);
    
    std::cout << "✓ If statement code generation test passed" << std::endl;
}

void testWhileStatement() {
    std::cout << "Testing while statement code generation..." << std::endl;
    
    Lexer lexer("while x < 10:\n    let x = x + 1\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasJumpIfFalse = false;
    bool hasJump = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::JUMP_IF_FALSE) {
            hasJumpIfFalse = true;
        }
        if (instr.opcode == OpCode::JUMP) {
            hasJump = true;
        }
    }
    assert(hasJumpIfFalse);
    assert(hasJump);
    
    std::cout << "✓ While statement code generation test passed" << std::endl;
}

void testRepeatStatement() {
    std::cout << "Testing repeat statement code generation..." << std::endl;
    
    Lexer lexer("repeat 3:\n    say \"hello\"\nend");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasDecareVar = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::DECLARE_VAR) {
            hasDecareVar = true;
            break;
        }
    }
    assert(hasDecareVar);
    
    std::cout << "✓ Repeat statement code generation test passed" << std::endl;
}

void testAssignment() {
    std::cout << "Testing assignment code generation..." << std::endl;
    
    Lexer lexer("x = 10");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasAssignVar = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::ASSIGN_VAR) {
            hasAssignVar = true;
            assert(instr.operands[0] == "x");
            break;
        }
    }
    assert(hasAssignVar);
    
    std::cout << "✓ Assignment code generation test passed" << std::endl;
}

void testUnaryExpression() {
    std::cout << "Testing unary expression code generation..." << std::endl;
    
    Lexer lexer("let result = -x");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    bool hasNegate = false;
    for (const auto& instr : bytecode) {
        if (instr.opcode == OpCode::NEGATE) {
            hasNegate = true;
            break;
        }
    }
    assert(hasNegate);
    
    std::cout << "✓ Unary expression code generation test passed" << std::endl;
}

void testStringLiteral() {
    std::cout << "Testing string literal code generation..." << std::endl;
    
    Lexer lexer("let message = \"Hello, World!\"");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    assert(bytecode[0].opcode == OpCode::PUSH_STRING);
    assert(bytecode[0].operands[0] == "Hello, World!");
    
    std::cout << "✓ String literal code generation test passed" << std::endl;
}

void testBooleanLiteral() {
    std::cout << "Testing boolean literal code generation..." << std::endl;
    
    Lexer lexer("let flag = true");
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    
    CodeGenerator generator;
    auto bytecode = generator.generate(ast.get());
    
    assert(bytecode[0].opcode == OpCode::PUSH_BOOLEAN);
    assert(bytecode[0].operands[0] == "true");
    
    std::cout << "✓ Boolean literal code generation test passed" << std::endl;
}

int main() {
    std::cout << "Running Code Generation Tests..." << std::endl;
    
    try {
        testVariableDeclaration();
        testBinaryExpression();
        testFunctionDeclaration();
        testFunctionCall();
        testBuiltinCall();
        testIfStatement();
        testWhileStatement();
        testRepeatStatement();
        testAssignment();
        testUnaryExpression();
        testStringLiteral();
        testBooleanLiteral();
        
        std::cout << "\n✅ All code generation tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
