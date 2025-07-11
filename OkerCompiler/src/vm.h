#ifndef VM_H
#define VM_H

#include "codegen.h"
#include <stack>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>

// Forward declare the BuiltinFunctions class to break the include cycle.
class BuiltinFunctions;
struct Value;

// A struct to represent a list in Oker.
struct OkerList {
    std::vector<Value> elements;
};
struct OkerDict {
    // A dictionary is a map from a string key to any Oker Value
    std::unordered_map<std::string, Value> pairs;
};

// The single, authoritative definition of a Value in Oker.
// It is a struct that inherits from std::variant to allow forward declaration.
struct Value : public std::variant<double, std::string, bool, std::shared_ptr<OkerList>, std::shared_ptr<OkerDict>> {
    // Inherit constructors from std::variant
    using variant::variant;
};



struct Function {
    std::string name;
    int address;
    std::vector<std::string> parameters;

    Function() : name(""), address(0), parameters() {}

    Function(const std::string& n, int addr, const std::vector<std::string>& params)
        : name(n), address(addr), parameters(params) {}
};

struct CallFrame {
    int returnAddress;
    std::unordered_map<std::string, Value> localVars;

    CallFrame(int retAddr) : returnAddress(retAddr) {}
};

struct TryFrame {
    int failAddress;
    size_t stackSize;
};

class VirtualMachine {
private:
    std::vector<Instruction> instructions;
    std::stack<Value> stack;
    std::stack<CallFrame> callStack;
    std::unordered_map<std::string, Value> globalVars;
    std::unordered_map<std::string, Function> functions;
    std::stack<TryFrame> tryStack;

    int pc;
    bool running;

    // Use a unique_ptr to the forward-declared class.
    // This must come AFTER all other members that might be used in its destructor.
    std::unique_ptr<BuiltinFunctions> builtins;

    void push(const Value& value);
    Value pop();
    Value peek();
    bool isEmpty();

    void setVariable(const std::string& name, const Value& value);
    Value getVariable(const std::string& name);

    void executeInstruction(const Instruction& instr);
    void executeBinaryOp(OpCode opcode);
    void executeUnaryOp(OpCode opcode);
    void executeComparison(OpCode opcode);
    void executeLogicalOp(OpCode opcode);
    void executeBuiltinCall(const std::string& name, int argCount);

public:
    // Public helpers for builtins
    std::string valueToString(const Value& value);
    double valueToNumber(const Value& value);
    bool valueToBoolean(const Value& value);

    VirtualMachine();
    ~VirtualMachine(); // Required for unique_ptr to incomplete type
    void execute(const std::vector<Instruction>& bytecode);
    void reset();

    // Debug methods
    void printStack();
    void printVariables();
};

#endif