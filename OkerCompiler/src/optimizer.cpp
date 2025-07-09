#include "optimizer.h"

Optimizer::Optimizer() {}

// This is the main entry point for the optimization phase.
// It takes the raw bytecode from the code generator.
std::vector<Instruction> Optimizer::optimize(const std::vector<Instruction>& bytecode) {
    // We start with a copy of the original bytecode.
    std::vector<Instruction> optimized_bytecode = bytecode;

    // In the future, you can add more optimization passes here.
    // For now, we only have one: optimizing increments/decrements.
    optimize_increments(optimized_bytecode);

    // Return the final, potentially smaller and faster, bytecode.
    return optimized_bytecode;
}


// This function implements the peephole optimization logic.
// It looks for a specific 4-instruction pattern and replaces it.
void Optimizer::optimize_increments(std::vector<Instruction>& bytecode) {
    std::vector<Instruction> result;
    result.reserve(bytecode.size()); // Reserve memory to avoid reallocations

    for (size_t i = 0; i < bytecode.size(); ++i) {
        // PEEPHOLE PATTERN: Check if we have at least 4 instructions left
        // to match the pattern for `x = x + 1` or `x = x - 1`.
        if (i + 3 < bytecode.size()) {
            const auto& instr1 = bytecode[i];
            const auto& instr2 = bytecode[i + 1];
            const auto& instr3 = bytecode[i + 2];
            const auto& instr4 = bytecode[i + 3];

            // Pattern: GET_VAR, PUSH_NUMBER 1, ADD/SUBTRACT, ASSIGN_VAR
            if (instr1.opcode == OpCode::GET_VAR &&
                instr2.opcode == OpCode::PUSH_NUMBER && instr2.operands[0] == "1.000000" &&
                (instr3.opcode == OpCode::ADD || instr3.opcode == OpCode::SUBTRACT) &&
                instr4.opcode == OpCode::ASSIGN_VAR &&
                instr1.operands[0] == instr4.operands[0]) { // Must be the same variable

                // We found a match!
                // Replace the 4 instructions with a single new one.
                if (instr3.opcode == OpCode::ADD) {
                    result.emplace_back(OpCode::INCREMENT, instr1.operands[0]);
                } else {
                    result.emplace_back(OpCode::DECREMENT, instr1.operands[0]);
                }

                // Skip the next 3 instructions since we've already processed them.
                i += 3;
                continue; // Continue to the next instruction
            }
        }

        // If no pattern was matched, just copy the original instruction.
        result.push_back(bytecode[i]);
    }

    // Replace the old bytecode with the newly optimized version.
    bytecode = result;
}
