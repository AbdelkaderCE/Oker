#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "codegen.h"
#include <vector>

// The Optimizer class is responsible for peephole optimizations.
// It scans the bytecode for inefficient patterns and replaces them
// with more efficient, specialized instructions.
class Optimizer {
public:
    Optimizer();

    // The main method that takes a vector of instructions and
    // returns a new, optimized vector of instructions.
    std::vector<Instruction> optimize(const std::vector<Instruction>& bytecode);

private:
    // Specific optimization patterns will be implemented as private methods.
    void optimize_increments(std::vector<Instruction>& optimized_bytecode);
};

#endif
