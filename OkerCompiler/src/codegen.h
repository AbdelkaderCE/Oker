#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <stack>

enum class OpCode {
    // Stack operations
    PUSH_NUMBER,
    PUSH_STRING,
    PUSH_BOOLEAN,
    PUSH_VARIABLE,

    // Arithmetic operations
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
    NEGATE,

    // Comparison operations
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,

    // Logical operations
    AND,
    OR,
    NOT,

    // Variable operations
    DECLARE_VAR,
    ASSIGN_VAR,
    GET_VAR,

    // Control flow
    JUMP,
    JUMP_IF_FALSE,
    JUMP_IF_TRUE,

    // Function operations
    CALL,
    RETURN,
    DEFINE_FUNCTION,

    // Built-in functions
    BUILTIN_CALL,

    // Utility
    POP,
    DUP,
    HALT,

    // Loop operations
    LOOP_START,
    LOOP_END,
    REPEAT_START,
    REPEAT_END,
    BREAK,
    CONTINUE,

    // List operations (New)
    BUILD_LIST,
    GET_INDEX,
    SET_INDEX,
    BUILD_DICT,

    // Optimized Opcodes
    INCREMENT,
    DECREMENT
};

struct Instruction {
    OpCode opcode;
    std::vector<std::string> operands;

    Instruction(OpCode op) : opcode(op) {}
    Instruction(OpCode op, const std::string& operand) : opcode(op) {
        operands.push_back(operand);
    }
    Instruction(OpCode op, const std::vector<std::string>& ops) : opcode(op), operands(ops) {}
};

struct LoopContext {
    std::string startLabel;
    std::string endLabel;
};

class CodeGenerator {
private:
    std::vector<Instruction> instructions;
    std::unordered_map<std::string, int> labelMap;
    int nextLabel;

    std::stack<LoopContext> loop_stack;

    void generateExpression(Expression* expr);
    void generateStatement(Statement* stmt);

    void generateBinaryExpression(BinaryExpression* expr);
    void generateUnaryExpression(UnaryExpression* expr);
    void generateCallExpression(CallExpression* expr);
    void generateIdentifier(Identifier* expr);
    void generateNumberLiteral(NumberLiteral* expr);
    void generateStringLiteral(StringLiteral* expr);
    void generateBooleanLiteral(BooleanLiteral* expr);
    void generateListLiteral(ListLiteral* expr);
    void generateIndexExpression(IndexExpression* expr);
    void generateDictLiteral(DictLiteral* expr);

    void generateVariableDeclaration(VariableDeclaration* stmt);
    void generateAssignment(Assignment* stmt);
    void generateFunctionDeclaration(FunctionDeclaration* stmt);
    void generateIfStatement(IfStatement* stmt);
    void generateWhileStatement(WhileStatement* stmt);
    void generateRepeatStatement(RepeatStatement* stmt);
    void generateReturnStatement(ReturnStatement* stmt);
    void generateBreakStatement(BreakStatement* stmt);
    void generateContinueStatement(ContinueStatement* stmt);
    void generateExpressionStatement(ExpressionStatement* stmt);

    void emit(OpCode opcode);
    void emit(OpCode opcode, const std::string& operand);
    void emit(OpCode opcode, const std::vector<std::string>& operands);

    std::string generateLabel();
    void markLabel(const std::string& label);
    void patchJumps(const std::string& label);
    void patchAllJumps();

public:
    CodeGenerator();

    std::vector<Instruction> generate(Program* program);
    void printBytecode(const std::vector<Instruction>& bytecode);
    std::string opcodeToString(OpCode opcode);
};

#endif
