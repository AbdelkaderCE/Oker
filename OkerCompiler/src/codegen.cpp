#include "codegen.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

CodeGenerator::CodeGenerator() : nextLabel(0) {}

std::vector<Instruction> CodeGenerator::generate(Program* program) {
    instructions.clear();
    labelMap.clear();
    nextLabel = 0;

    for (auto& stmt : program->statements) {
        generateStatement(stmt.get());
    }

    emit(OpCode::HALT);
    patchAllJumps();
    return instructions;
}

void CodeGenerator::generateStatement(Statement* stmt) {
    switch (stmt->type) {
        case NodeType::VARIABLE_DECLARATION:
            generateVariableDeclaration(static_cast<VariableDeclaration*>(stmt));
            break;
        case NodeType::ASSIGNMENT:
            generateAssignment(static_cast<Assignment*>(stmt));
            break;
        case NodeType::FUNCTION_DECLARATION:
            generateFunctionDeclaration(static_cast<FunctionDeclaration*>(stmt));
            break;
        case NodeType::IF_STATEMENT:
            generateIfStatement(static_cast<IfStatement*>(stmt));
            break;
        case NodeType::WHILE_STATEMENT:
            generateWhileStatement(static_cast<WhileStatement*>(stmt));
            break;
        case NodeType::REPEAT_STATEMENT:
            generateRepeatStatement(static_cast<RepeatStatement*>(stmt));
            break;
        case NodeType::RETURN_STATEMENT:
            generateReturnStatement(static_cast<ReturnStatement*>(stmt));
            break;
        case NodeType::BREAK_STATEMENT:
            generateBreakStatement(static_cast<BreakStatement*>(stmt));
            break;
        case NodeType::CONTINUE_STATEMENT:
            generateContinueStatement(static_cast<ContinueStatement*>(stmt));
            break;
        case NodeType::EXPRESSION_STATEMENT:
            generateExpressionStatement(static_cast<ExpressionStatement*>(stmt));
            break;
        default:
            break;
    }
}

void CodeGenerator::generateExpression(Expression* expr) {
    switch (expr->type) {
        case NodeType::NUMBER_LITERAL:
            generateNumberLiteral(static_cast<NumberLiteral*>(expr));
            break;
        case NodeType::STRING_LITERAL:
            generateStringLiteral(static_cast<StringLiteral*>(expr));
            break;
        case NodeType::BOOLEAN_LITERAL:
            generateBooleanLiteral(static_cast<BooleanLiteral*>(expr));
            break;
        case NodeType::IDENTIFIER:
            generateIdentifier(static_cast<Identifier*>(expr));
            break;
        case NodeType::BINARY_EXPRESSION:
            generateBinaryExpression(static_cast<BinaryExpression*>(expr));
            break;
        case NodeType::UNARY_EXPRESSION:
            generateUnaryExpression(static_cast<UnaryExpression*>(expr));
            break;
        case NodeType::CALL_EXPRESSION:
            generateCallExpression(static_cast<CallExpression*>(expr));
            break;
        case NodeType::LIST_LITERAL:
            generateListLiteral(static_cast<ListLiteral*>(expr));
            break;
        case NodeType::INDEX_EXPRESSION:
            generateIndexExpression(static_cast<IndexExpression*>(expr));
            break;
        // This case was missing, causing the linker error.
        case NodeType::DICT_LITERAL:
            generateDictLiteral(static_cast<DictLiteral*>(expr));
            break;
        default:
            break;
    }
}

void CodeGenerator::generateDictLiteral(DictLiteral* expr) {
    for (size_t i = 0; i < expr->keys.size(); ++i) {
        generateExpression(expr->keys[i].get());
        generateExpression(expr->values[i].get());
    }
    emit(OpCode::BUILD_DICT, std::to_string(expr->keys.size()));
}


void CodeGenerator::generateBinaryExpression(BinaryExpression* expr) {
    generateExpression(expr->left.get());
    generateExpression(expr->right.get());

    switch (expr->operator_) {
        case TokenType::PLUS:       emit(OpCode::ADD); break;
        case TokenType::MINUS:      emit(OpCode::SUBTRACT); break;
        case TokenType::MULTIPLY:   emit(OpCode::MULTIPLY); break;
        case TokenType::DIVIDE:     emit(OpCode::DIVIDE); break;
        case TokenType::MODULO:     emit(OpCode::MODULO); break;
        case TokenType::EQUAL:      emit(OpCode::EQUAL); break;
        case TokenType::NOT_EQUAL:  emit(OpCode::NOT_EQUAL); break;
        case TokenType::LESS_THAN:  emit(OpCode::LESS_THAN); break;
        case TokenType::LESS_EQUAL: emit(OpCode::LESS_EQUAL); break;
        case TokenType::GREATER_THAN: emit(OpCode::GREATER_THAN); break;
        case TokenType::GREATER_EQUAL: emit(OpCode::GREATER_EQUAL); break;
        case TokenType::AND:        emit(OpCode::AND); break;
        case TokenType::OR:         emit(OpCode::OR); break;
        default: break;
    }
}

void CodeGenerator::generateUnaryExpression(UnaryExpression* expr) {
    generateExpression(expr->operand.get());

    switch (expr->operator_) {
        case TokenType::MINUS: emit(OpCode::NEGATE); break;
        case TokenType::NOT:   emit(OpCode::NOT); break;
        default: break;
    }
}

void CodeGenerator::generateCallExpression(CallExpression* expr) {
    for (auto it = expr->arguments.rbegin(); it != expr->arguments.rend(); ++it) {
        generateExpression(it->get());
    }

    if (expr->callee->type == NodeType::IDENTIFIER) {
        Identifier* callee = static_cast<Identifier*>(expr->callee.get());

        if (callee->name == "say" || callee->name == "input" || 
            callee->name == "str" || callee->name == "num" || 
            callee->name == "bool" || callee->name == "len" ||
            callee->name == "type" || callee->name == "abs" ||
            callee->name == "max" || callee->name == "min" ||
            callee->name == "round" || callee->name == "sqrt" ||
            callee->name == "pow" || callee->name == "random" ||
            callee->name == "upper" || callee->name == "lower" ||
            callee->name == "strip" || callee->name == "split_str" ||
            callee->name == "replace_str" ||callee->name == "charAt" || 
            callee->name == "sbuild_new" ||
            callee->name == "sbuild_add" || callee->name == "sbuild_get" ||
            callee->name == "list_add" ||
            callee->name == "exists" || callee->name == "listdir" ||
            callee->name == "exit" || callee->name == "sleep" ||
            callee->name == "get" || callee->name == "save" ||
            callee->name == "deletef") {

            emit(OpCode::BUILTIN_CALL, {callee->name, std::to_string(expr->arguments.size())});
        } else {
            emit(OpCode::CALL, {callee->name, std::to_string(expr->arguments.size())});
        }
    }
}

void CodeGenerator::generateIdentifier(Identifier* expr) {
    emit(OpCode::GET_VAR, expr->name);
}

void CodeGenerator::generateNumberLiteral(NumberLiteral* expr) {
    emit(OpCode::PUSH_NUMBER, std::to_string(expr->value));
}

void CodeGenerator::generateStringLiteral(StringLiteral* expr) {
    emit(OpCode::PUSH_STRING, expr->value);
}

void CodeGenerator::generateBooleanLiteral(BooleanLiteral* expr) {
    emit(OpCode::PUSH_BOOLEAN, expr->value ? "true" : "false");
}

void CodeGenerator::generateListLiteral(ListLiteral* expr) {
    // Generate code for each element in reverse order
    for (auto it = expr->elements.rbegin(); it != expr->elements.rend(); ++it) {
        generateExpression(it->get());
    }
    // Emit the instruction to build the list from the stack
    emit(OpCode::BUILD_LIST, std::to_string(expr->elements.size()));
}

void CodeGenerator::generateIndexExpression(IndexExpression* expr) {
    // Generate code for the object (the list)
    generateExpression(expr->object.get());
    // Generate code for the index
    generateExpression(expr->index.get());
    // Emit the instruction to get the value at the index
    emit(OpCode::GET_INDEX);
}

void CodeGenerator::generateVariableDeclaration(VariableDeclaration* stmt) {
    if (stmt->initializer) {
        generateExpression(stmt->initializer.get());
    } else {
        emit(OpCode::PUSH_NUMBER, "0");
    }
    emit(OpCode::DECLARE_VAR, stmt->name);
}

void CodeGenerator::generateAssignment(Assignment* stmt) {
    // Check if the target is an index expression (e.g., my_list[0] = ...)
    if (stmt->target->type == NodeType::INDEX_EXPRESSION) {
        IndexExpression* target = static_cast<IndexExpression*>(stmt->target.get());

        // Push the new value onto the stack first
        generateExpression(stmt->value.get());
        // Then push the object (the list)
        generateExpression(target->object.get());
        // Then push the index
        generateExpression(target->index.get());
        // Finally, emit the SET_INDEX instruction
        emit(OpCode::SET_INDEX);
    } else {
        // Handle regular variable assignment
        generateExpression(stmt->value.get());
        if (stmt->target->type == NodeType::IDENTIFIER) {
            Identifier* target = static_cast<Identifier*>(stmt->target.get());
            emit(OpCode::ASSIGN_VAR, target->name);
        }
    }
}

void CodeGenerator::generateFunctionDeclaration(FunctionDeclaration* stmt) {
    std::string funcStartLabel = generateLabel();
    std::string funcEndLabel = generateLabel();

    emit(OpCode::JUMP, funcEndLabel);

    markLabel(funcStartLabel);
    int funcStartAddress = instructions.size();

    for (auto& bodyStmt : stmt->body) {
        generateStatement(bodyStmt.get());
    }

    emit(OpCode::PUSH_NUMBER, "0");
    emit(OpCode::RETURN);

    markLabel(funcEndLabel);

    std::vector<std::string> operands = {stmt->name, std::to_string(funcStartAddress), std::to_string(stmt->parameters.size())};
    for (const auto& param : stmt->parameters) {
        operands.push_back(param);
    }
    emit(OpCode::DEFINE_FUNCTION, operands);
}

void CodeGenerator::generateIfStatement(IfStatement* stmt) {
    std::string elseLabel = generateLabel();
    std::string endLabel = generateLabel();

    generateExpression(stmt->condition.get());
    emit(OpCode::JUMP_IF_FALSE, elseLabel);

    for (auto& thenStmt : stmt->thenBranch) {
        generateStatement(thenStmt.get());
    }

    if (!stmt->elseBranch.empty()) {
        emit(OpCode::JUMP, endLabel);
    }

    markLabel(elseLabel);

    for (auto& elseStmt : stmt->elseBranch) {
        generateStatement(elseStmt.get());
    }

    markLabel(endLabel);
}

void CodeGenerator::generateWhileStatement(WhileStatement* stmt) {
    std::string loopStart = generateLabel();
    std::string loopEnd = generateLabel();

    loop_stack.push({loopStart, loopEnd});

    markLabel(loopStart);

    generateExpression(stmt->condition.get());
    emit(OpCode::JUMP_IF_FALSE, loopEnd);

    for (auto& bodyStmt : stmt->body) {
        generateStatement(bodyStmt.get());
    }

    emit(OpCode::JUMP, loopStart);

    markLabel(loopEnd);
    loop_stack.pop();
}

void CodeGenerator::generateRepeatStatement(RepeatStatement* stmt) {
    std::string loopStart = generateLabel();
    std::string loopEnd = generateLabel();
    std::string counterVar = "__repeat_counter_" + std::to_string(nextLabel++);

    loop_stack.push({loopStart, loopEnd});

    generateExpression(stmt->count.get());
    emit(OpCode::DECLARE_VAR, counterVar);

    markLabel(loopStart);

    emit(OpCode::GET_VAR, counterVar);
    emit(OpCode::PUSH_NUMBER, "0");
    emit(OpCode::GREATER_THAN);
    emit(OpCode::JUMP_IF_FALSE, loopEnd);

    for (auto& bodyStmt : stmt->body) {
        generateStatement(bodyStmt.get());
    }

    emit(OpCode::GET_VAR, counterVar);
    emit(OpCode::PUSH_NUMBER, "1");
    emit(OpCode::SUBTRACT);
    emit(OpCode::ASSIGN_VAR, counterVar);

    emit(OpCode::JUMP, loopStart);

    markLabel(loopEnd);
    loop_stack.pop();
}

void CodeGenerator::generateReturnStatement(ReturnStatement* stmt) {
    if (stmt->value) {
        generateExpression(stmt->value.get());
    } else {
        emit(OpCode::PUSH_NUMBER, "0");
    }
    emit(OpCode::RETURN);
}

void CodeGenerator::generateBreakStatement(BreakStatement* stmt) {
    if (loop_stack.empty()) {
        throw std::runtime_error("'break' statement outside of a loop.");
    }
    emit(OpCode::JUMP, loop_stack.top().endLabel);
}

void CodeGenerator::generateContinueStatement(ContinueStatement* stmt) {
    if (loop_stack.empty()) {
        throw std::runtime_error("'continue' statement outside of a loop.");
    }
    emit(OpCode::JUMP, loop_stack.top().startLabel);
}

void CodeGenerator::generateExpressionStatement(ExpressionStatement* stmt) {
    generateExpression(stmt->expression.get());
    emit(OpCode::POP);
}

void CodeGenerator::emit(OpCode opcode) {
    instructions.emplace_back(opcode);
}

void CodeGenerator::emit(OpCode opcode, const std::string& operand) {
    instructions.emplace_back(opcode, operand);
}

void CodeGenerator::emit(OpCode opcode, const std::vector<std::string>& operands) {
    instructions.emplace_back(opcode, operands);
}

std::string CodeGenerator::generateLabel() {
    return "__label_" + std::to_string(nextLabel++);
}

void CodeGenerator::markLabel(const std::string& label) {
    labelMap[label] = instructions.size();
}

void CodeGenerator::patchJumps(const std::string& label) {
    if (labelMap.find(label) == labelMap.end()) return;
    int targetAddress = labelMap[label];

    for (auto& instr : instructions) {
        if ((instr.opcode == OpCode::JUMP || instr.opcode == OpCode::JUMP_IF_FALSE || 
             instr.opcode == OpCode::JUMP_IF_TRUE) && 
            !instr.operands.empty() && instr.operands[0] == label) {
            instr.operands[0] = std::to_string(targetAddress);
        }
    }
}

void CodeGenerator::patchAllJumps() {
    for (auto& instr : instructions) {
        if ((instr.opcode == OpCode::JUMP || instr.opcode == OpCode::JUMP_IF_FALSE || 
             instr.opcode == OpCode::JUMP_IF_TRUE) && !instr.operands.empty()) {
            const std::string& label = instr.operands[0];
            if (labelMap.count(label)) {
                instr.operands[0] = std::to_string(labelMap[label]);
            }
        }
    }
}

void CodeGenerator::printBytecode(const std::vector<Instruction>& bytecode) {
    for (size_t i = 0; i < bytecode.size(); i++) {
        const auto& instr = bytecode[i];
        std::cout << i << ": " << opcodeToString(instr.opcode);

        for (const auto& operand : instr.operands) {
            std::cout << " " << operand;
        }

        std::cout << "\n";
    }
}

std::string CodeGenerator::opcodeToString(OpCode opcode) {
    switch (opcode) {
        case OpCode::PUSH_NUMBER: return "PUSH_NUMBER";
        case OpCode::PUSH_STRING: return "PUSH_STRING";
        case OpCode::PUSH_BOOLEAN: return "PUSH_BOOLEAN";
        case OpCode::PUSH_VARIABLE: return "PUSH_VARIABLE";
        case OpCode::ADD: return "ADD";
        case OpCode::SUBTRACT: return "SUBTRACT";
        case OpCode::MULTIPLY: return "MULTIPLY";
        case OpCode::DIVIDE: return "DIVIDE";
        case OpCode::MODULO: return "MODULO";
        case OpCode::NEGATE: return "NEGATE";
        case OpCode::EQUAL: return "EQUAL";
        case OpCode::NOT_EQUAL: return "NOT_EQUAL";
        case OpCode::LESS_THAN: return "LESS_THAN";
        case OpCode::LESS_EQUAL: return "LESS_EQUAL";
        case OpCode::GREATER_THAN: return "GREATER_THAN";
        case OpCode::GREATER_EQUAL: return "GREATER_EQUAL";
        case OpCode::AND: return "AND";
        case OpCode::OR: return "OR";
        case OpCode::NOT: return "NOT";
        case OpCode::DECLARE_VAR: return "DECLARE_VAR";
        case OpCode::ASSIGN_VAR: return "ASSIGN_VAR";
        case OpCode::GET_VAR: return "GET_VAR";
        case OpCode::JUMP: return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::JUMP_IF_TRUE: return "JUMP_IF_TRUE";
        case OpCode::CALL: return "CALL";
        case OpCode::RETURN: return "RETURN";
        case OpCode::DEFINE_FUNCTION: return "DEFINE_FUNCTION";
        case OpCode::BUILTIN_CALL: return "BUILTIN_CALL";
        case OpCode::POP: return "POP";
        case OpCode::DUP: return "DUP";
        case OpCode::HALT: return "HALT";
        case OpCode::LOOP_START: return "LOOP_START";
        case OpCode::LOOP_END: return "LOOP_END";
        case OpCode::REPEAT_START: return "REPEAT_START";
        case OpCode::REPEAT_END: return "REPEAT_END";
        case OpCode::BREAK: return "BREAK";
        case OpCode::CONTINUE: return "CONTINUE";
        case OpCode::BUILD_LIST: return "BUILD_LIST";
        case OpCode::GET_INDEX: return "GET_INDEX";
        case OpCode::SET_INDEX: return "SET_INDEX";
        case OpCode::BUILD_DICT: return "BUILD_DICT";
        case OpCode::INCREMENT: return "INCREMENT";
        case OpCode::DECREMENT: return "DECREMENT";
        default: return "UNKNOWN";
    }
}