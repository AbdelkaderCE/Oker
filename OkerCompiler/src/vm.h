#ifndef VM_H
#define VM_H

#include "codegen.h"
#include <stack>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>

// Forward declare classes to break include cycles
class BuiltinFunctions;
struct Value;
struct OkerInstance;
struct Function; 

// A struct to represent a list in Oker.
struct OkerList {
    std::vector<Value> elements;
};
struct OkerDict {
    std::unordered_map<std::string, Value> pairs;
};

// Represents the blueprint of a class
struct OkerClass {
    std::string name;
    std::unordered_map<std::string, Function> methods;

    OkerClass(const std::string& n) : name(n) {}
};

// Represents an instance of a class
struct OkerInstance {
    std::shared_ptr<OkerClass> klass;
    std::unordered_map<std::string, Value> fields;

    OkerInstance(std::shared_ptr<OkerClass> k) : klass(k) {}
};


// The single, authoritative definition of a Value in Oker.
struct Value : public std::variant<
    double, 
    std::string, 
    bool, 
    std::shared_ptr<OkerList>, 
    std::shared_ptr<OkerDict>,
    std::shared_ptr<OkerClass>,     
    std::shared_ptr<OkerInstance>   
> {
    using variant::variant;
};

struct Function {
    std::string name;
    int address;
    std::vector<std::string> parameters;
    bool isMethod = false;

    Function() : name(""), address(0), parameters() {}

    Function(const std::string& n, int addr, const std::vector<std::string>& params, bool is_method = false)
        : name(n), address(addr), parameters(params), isMethod(is_method) {}
};

struct CallFrame {
    int returnAddress;
    std::unordered_map<std::string, Value> localVars;
    std::shared_ptr<OkerInstance> self = nullptr;

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
    std::unordered_map<std::string, std::shared_ptr<OkerClass>> classes;
    std::stack<TryFrame> tryStack;

    int pc;
    bool running;

    std::unique_ptr<BuiltinFunctions> builtins;

    void push(const Value& value);
    Value pop();
    Value peek();
    Value peek(int distance); // <--- THIS IS THE MISSING DECLARATION
    bool isEmpty();

    void setVariable(const std::string& name, const Value& value);
    Value getVariable(const std::string& name);
    void callFunction(Function& func, int argCount);

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
    ~VirtualMachine(); 
    void execute(const std::vector<Instruction>& bytecode);
    void reset();

    // Debug methods
    void printStack();
    void printVariables();
};

#endif