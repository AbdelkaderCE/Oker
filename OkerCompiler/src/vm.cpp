#include "vm.h"
#include "builtins.h" 
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cmath>

VirtualMachine::VirtualMachine() : pc(0), running(false), builtins(std::make_unique<BuiltinFunctions>()) {}

VirtualMachine::~VirtualMachine() = default;

void VirtualMachine::execute(const std::vector<Instruction>& bytecode) {
    instructions = bytecode;
    pc = 0;
    running = true;

    while (running && pc < static_cast<int>(instructions.size())) {
        try {
            executeInstruction(instructions[pc]);
            pc++;
        } catch (const std::runtime_error& e) {
            if (!tryStack.empty()) {
                pc = tryStack.top().failAddress;
                while (stack.size() > tryStack.top().stackSize) {
                    stack.pop();
                }
            } else {
                std::cerr << "Runtime Error: " << e.what() << " at instruction " << pc << std::endl;
                running = false;
            }
        }
    }
}

void VirtualMachine::reset() {
    while (!stack.empty()) stack.pop();
    while (!callStack.empty()) callStack.pop();
    globalVars.clear();
    functions.clear();
    classes.clear();
    pc = 0;
    running = false;
}

void VirtualMachine::push(const Value& value) {
    stack.push(value);
}

Value VirtualMachine::pop() {
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    Value value = stack.top();
    stack.pop();
    return value;
}

Value VirtualMachine::peek() {
    if (stack.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack.top();
}

// Function to peek deeper into the stack
Value VirtualMachine::peek(int distance) {
    if (stack.size() < distance + 1) {
        throw std::runtime_error("Stack underflow on peek.");
    }
    std::stack<Value> temp = stack;
    for (int i = 0; i < distance; i++) {
        temp.pop();
    }
    return temp.top();
}


void VirtualMachine::setVariable(const std::string& name, const Value& value) {
    if (!callStack.empty()) {
        auto& localVars = callStack.top().localVars;
        if (localVars.count(name)) {
            localVars[name] = value;
            return;
        }
    }
    globalVars[name] = value;
}

Value VirtualMachine::getVariable(const std::string& name) {
    if (!callStack.empty()) {
        auto& localVars = callStack.top().localVars;
        auto it = localVars.find(name);
        if (it != localVars.end()) {
            return it->second;
        }
    }

    auto it = globalVars.find(name);
    if (it != globalVars.end()) {
        return it->second;
    }

    throw std::runtime_error("Undefined variable: " + name);
}

void VirtualMachine::callFunction(Function& func, int argCount) {
    CallFrame frame(pc);

    std::vector<Value> args;
    for (int i = 0; i < argCount; i++) {
        args.push_back(pop());
    }

    if (func.isMethod) {
        frame.self = std::get<std::shared_ptr<OkerInstance>>(pop());
    }

    for (size_t i = 0; i < func.parameters.size(); i++) {
        if (i < args.size()) {
            frame.localVars[func.parameters[i]] = args[argCount - 1 - i];
        }
    }

    callStack.push(frame);
    pc = func.address - 1;
}

void VirtualMachine::executeInstruction(const Instruction& instr) {
    switch (instr.opcode) {
        case OpCode::TRY_START: {
            int failAddr = std::stoi(instr.operands[0]);
            tryStack.push({failAddr, stack.size()});
            break;
        }

        case OpCode::TRY_END: {
            if (!tryStack.empty()) {
                tryStack.pop();
            }
            break;
        }
        case OpCode::PUSH_NUMBER:
            push(Value(std::stod(instr.operands[0])));
            break;

        case OpCode::PUSH_STRING:
            push(Value(instr.operands[0]));
            break;

        case OpCode::PUSH_BOOLEAN:
            push(Value(instr.operands[0] == "true"));
            break;

        case OpCode::GET_VAR:
            push(getVariable(instr.operands[0]));
            break;

        case OpCode::DECLARE_VAR:
            if (!callStack.empty()) {
                callStack.top().localVars[instr.operands[0]] = pop();
            } else {
                globalVars[instr.operands[0]] = pop();
            }
            break;

        case OpCode::ASSIGN_VAR:
            setVariable(instr.operands[0], pop());
            break;

        case OpCode::ADD:
        case OpCode::SUBTRACT:
        case OpCode::MULTIPLY:
        case OpCode::DIVIDE:
        case OpCode::MODULO:
            executeBinaryOp(instr.opcode);
            break;

        case OpCode::NEGATE:
        case OpCode::NOT:
            executeUnaryOp(instr.opcode);
            break;

        case OpCode::EQUAL:
        case OpCode::NOT_EQUAL:
        case OpCode::LESS_THAN:
        case OpCode::LESS_EQUAL:
        case OpCode::GREATER_THAN:
        case OpCode::GREATER_EQUAL:
            executeComparison(instr.opcode);
            break;

        case OpCode::AND:
        case OpCode::OR:
            executeLogicalOp(instr.opcode);
            break;

        case OpCode::JUMP:
            pc = std::stoi(instr.operands[0]) - 1;
            break;

        case OpCode::JUMP_IF_FALSE:
            if (!valueToBoolean(pop())) {
                pc = std::stoi(instr.operands[0]) - 1;
            }
            break;

        case OpCode::JUMP_IF_TRUE:
            if (valueToBoolean(pop())) {
                pc = std::stoi(instr.operands[0]) - 1;
            }
            break;

        case OpCode::DEFINE_FUNCTION: {
            std::string name = instr.operands[0];
            int address = std::stoi(instr.operands[1]);
            int paramCount = std::stoi(instr.operands[2]);

            std::vector<std::string> params;
            for (int i = 3; i < 3 + paramCount; i++) {
                params.push_back(instr.operands[i]);
            }

            size_t dotPos = name.find('.');
            if (dotPos != std::string::npos) {
                std::string className = name.substr(0, dotPos);
                std::string methodName = name.substr(dotPos + 1);
                classes.at(className)->methods[methodName] = Function(methodName, address, params, true);
            } else {
                functions[name] = Function(name, address, params);
            }
            break;
        }

        case OpCode::CALL: {
            std::string funcName = instr.operands[0];
            int argCount = std::stoi(instr.operands[1]);

            Value calleeContext = peek(argCount); 

            if (std::holds_alternative<std::shared_ptr<OkerInstance>>(calleeContext)) {
                auto instance = std::get<std::shared_ptr<OkerInstance>>(calleeContext);
                auto it = instance->klass->methods.find(funcName);
                if (it == instance->klass->methods.end()) {
                    throw std::runtime_error("Undefined method '" + funcName + "' on class " + instance->klass->name);
                }
                callFunction(it->second, argCount);
            } else {
                auto it = functions.find(funcName);
                if (it == functions.end()) {
                    throw std::runtime_error("Cannot call non-function: " + funcName);
                }
                callFunction(it->second, argCount);
            }
            break;
        }

        // CORRECTED RETURN LOGIC
        case OpCode::RETURN: {
            Value returnValue = pop();

            if (callStack.empty()) {
                throw std::runtime_error("Return outside function");
            }

            bool wasMethod = callStack.top().self != nullptr;

            int returnAddr = callStack.top().returnAddress;
            callStack.pop();

            // If the call was a method, pop the instance ('this') from the stack
            if (wasMethod) {
                pop(); 
            }

            push(returnValue);
            pc = returnAddr;
            break;
        }

        case OpCode::BUILTIN_CALL: {
            std::string funcName = instr.operands[0];
            int argCount = std::stoi(instr.operands[1]);
            executeBuiltinCall(funcName, argCount);
            break;
        }

        case OpCode::BUILD_LIST: {
            int elementCount = std::stoi(instr.operands[0]);
            auto list = std::make_shared<OkerList>();
            for (int i = 0; i < elementCount; ++i) {
                list->elements.push_back(pop());
            }
            std::reverse(list->elements.begin(), list->elements.end());
            push(Value(list));
            break;
        }

        case OpCode::BUILD_DICT: {
            int pairCount = std::stoi(instr.operands[0]);
            auto dict = std::make_shared<OkerDict>();
            for (int i = 0; i < pairCount; ++i) {
                Value val = pop();
                Value key = pop();
                dict->pairs[valueToString(key)] = val;
            }
            push(Value(dict));
            break;
        }

        case OpCode::GET_INDEX: {
            Value indexVal = pop();
            Value containerVal = pop();

            if (std::holds_alternative<std::shared_ptr<OkerList>>(containerVal)) {
                auto list = std::get<std::shared_ptr<OkerList>>(containerVal);
                int index = static_cast<int>(valueToNumber(indexVal));
                if (index < 0 || index >= static_cast<int>(list->elements.size())) {
                    throw std::runtime_error("List index out of bounds.");
                }
                push(list->elements.at(index));
            } else if (std::holds_alternative<std::shared_ptr<OkerDict>>(containerVal)) {
                auto dict = std::get<std::shared_ptr<OkerDict>>(containerVal);
                std::string key = valueToString(indexVal);
                if (dict->pairs.find(key) == dict->pairs.end()) {
                    throw std::runtime_error("Dictionary key not found: " + key);
                }
                push(dict->pairs.at(key));
            } else {
                throw std::runtime_error("Cannot index a non-list/non-dictionary type.");
            }
            break;
        }

        case OpCode::SET_INDEX: {
            Value newValue = pop();
            Value indexVal = pop();
            Value containerVal = pop();

            if (std::holds_alternative<std::shared_ptr<OkerList>>(containerVal)) {
                auto list = std::get<std::shared_ptr<OkerList>>(containerVal);
                int index = static_cast<int>(valueToNumber(indexVal));
                if (index < 0 || index >= static_cast<int>(list->elements.size())) {
                    throw std::runtime_error("List index out of bounds.");
                }
                list->elements[index] = newValue;
            } else if (std::holds_alternative<std::shared_ptr<OkerDict>>(containerVal)) {
                auto dict = std::get<std::shared_ptr<OkerDict>>(containerVal);
                std::string key = valueToString(indexVal);
                dict->pairs[key] = newValue;
            } else {
                 throw std::runtime_error("Cannot set index on a non-list/non-dictionary type.");
            }
            push(newValue);
            break;
        }

        case OpCode::DEFINE_CLASS: {
            classes[instr.operands[0]] = std::make_shared<OkerClass>(instr.operands[0]);
            break;
        }
        case OpCode::CREATE_INSTANCE: {
            std::string className = instr.operands[0];
            int argCount = std::stoi(instr.operands[1]);

            auto it = classes.find(className);
            if (it == classes.end()) {
                throw std::runtime_error("Class '" + className + "' not defined.");
            }

            auto instance = std::make_shared<OkerInstance>(it->second);
            push(Value(instance));

            auto ctor_it = it->second->methods.find(className);
            if (ctor_it != it->second->methods.end()) {
                callFunction(ctor_it->second, argCount);
            }
            break;
        }
        case OpCode::GET_PROPERTY: {
            Value objectVal = pop();
             if (!std::holds_alternative<std::shared_ptr<OkerInstance>>(objectVal)) {
                throw std::runtime_error("Can only get properties of instances.");
            }
            auto instance = std::get<std::shared_ptr<OkerInstance>>(objectVal);
            auto it = instance->fields.find(instr.operands[0]);
            if (it != instance->fields.end()) {
                push(it->second);
            } else {
                 auto method_it = instance->klass->methods.find(instr.operands[0]);
                if (method_it == instance->klass->methods.end()) {
                    throw std::runtime_error("Undefined property '" + instr.operands[0] + "' on instance of " + instance->klass->name);
                }
                push(objectVal);
            }
            break;
        }
        case OpCode::SET_PROPERTY: {
            Value value = pop();
            Value objectVal = pop();
            if (!std::holds_alternative<std::shared_ptr<OkerInstance>>(objectVal)) {
                throw std::runtime_error("Can only set properties on instances.");
            }
            auto instance = std::get<std::shared_ptr<OkerInstance>>(objectVal);
            instance->fields[instr.operands[0]] = value;
            push(value);
            break;
        }
        case OpCode::GET_THIS: {
            if (callStack.empty() || !callStack.top().self) {
                throw std::runtime_error("'this' can only be used inside a method.");
            }
            push(Value(callStack.top().self));
            break;
        }


        case OpCode::POP:
            pop();
            break;

        case OpCode::DUP:
            push(peek());
            break;

        case OpCode::HALT:
            running = false;
            break;

        case OpCode::INCREMENT: {
            const std::string& varName = instr.operands[0];
            Value val = getVariable(varName);
            setVariable(varName, Value(valueToNumber(val) + 1.0));
            break;
        }

        case OpCode::DECREMENT: {
            const std::string& varName = instr.operands[0];
            Value val = getVariable(varName);
            setVariable(varName, Value(valueToNumber(val) - 1.0));
            break;
        }

        default:
            throw std::runtime_error("Unknown opcode: " + std::to_string(static_cast<int>(instr.opcode)));
    }
}

void VirtualMachine::executeBinaryOp(OpCode opcode) {
    Value right = pop();
    Value left = pop();

    switch (opcode) {
        case OpCode::ADD:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                push(Value(std::get<double>(left) + std::get<double>(right)));
            } else if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
                push(Value(valueToString(left) + valueToString(right)));
            } else {
                 push(Value(valueToNumber(left) + valueToNumber(right)));
            }
            break;
        case OpCode::SUBTRACT:
            push(Value(valueToNumber(left) - valueToNumber(right)));
            break;
        case OpCode::MULTIPLY:
            push(Value(valueToNumber(left) * valueToNumber(right)));
            break;
        case OpCode::DIVIDE:
            if (valueToNumber(right) == 0) throw std::runtime_error("Division by zero");
            push(Value(valueToNumber(left) / valueToNumber(right)));
            break;
        case OpCode::MODULO:
            if (valueToNumber(right) == 0) throw std::runtime_error("Modulo by zero");
            push(Value(fmod(valueToNumber(left), valueToNumber(right))));
            break;
        default:
            throw std::runtime_error("Unknown binary operation");
    }
}

void VirtualMachine::executeUnaryOp(OpCode opcode) {
    Value operand = pop();
    switch (opcode) {
        case OpCode::NEGATE:
            push(Value(-valueToNumber(operand)));
            break;
        case OpCode::NOT:
            push(Value(!valueToBoolean(operand)));
            break;
        default:
            throw std::runtime_error("Unknown unary operation");
    }
}

void VirtualMachine::executeComparison(OpCode opcode) {
    Value right = pop();
    Value left = pop();

    if (opcode != OpCode::EQUAL && opcode != OpCode::NOT_EQUAL) {
        double leftNum = valueToNumber(left);
        double rightNum = valueToNumber(right);
        switch (opcode) {
            case OpCode::LESS_THAN: push(Value(leftNum < rightNum)); break;
            case OpCode::LESS_EQUAL: push(Value(leftNum <= rightNum)); break;
            case OpCode::GREATER_THAN: push(Value(leftNum > rightNum)); break;
            case OpCode::GREATER_EQUAL: push(Value(leftNum >= rightNum)); break;
            default: break;
        }
        return;
    }

    if (left.index() == right.index()) {
         switch (opcode) {
            case OpCode::EQUAL: push(Value(left == right)); break;
            case OpCode::NOT_EQUAL: push(Value(left != right)); break;
            default: break;
        }
    } else {
         switch (opcode) {
            case OpCode::EQUAL: push(Value(valueToString(left) == valueToString(right))); break;
            case OpCode::NOT_EQUAL: push(Value(valueToString(left) != valueToString(right))); break;
            default: break;
        }
    }
}

void VirtualMachine::executeLogicalOp(OpCode opcode) {
    Value right = pop();
    Value left = pop();
    switch (opcode) {
        case OpCode::AND:
            push(Value(valueToBoolean(left) && valueToBoolean(right)));
            break;
        case OpCode::OR:
            push(Value(valueToBoolean(left) || valueToBoolean(right)));
            break;
        default:
            throw std::runtime_error("Unknown logical operation");
    }
}

void VirtualMachine::executeBuiltinCall(const std::string& name, int argCount) {
    std::vector<Value> args;
    for (int i = 0; i < argCount; i++) {
        args.push_back(pop());
    }
    std::reverse(args.begin(), args.end());
    push(builtins->call(name, args, *this));
}

std::string VirtualMachine::valueToString(const Value& value) {
    if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::shared_ptr<OkerList>>(value)) {
        auto list = std::get<std::shared_ptr<OkerList>>(value);
        std::string result = "[";
        for (size_t i = 0; i < list->elements.size(); ++i) {
            result += valueToString(list->elements[i]);
            if (i < list->elements.size() - 1) {
                result += ", ";
            }
        }
        result += "]";
        return result;
    } else if (std::holds_alternative<std::shared_ptr<OkerDict>>(value)) {
        auto dict = std::get<std::shared_ptr<OkerDict>>(value);
        std::string result = "{";
        auto it = dict->pairs.begin();
        while (it != dict->pairs.end()) {
            result += "\"" + it->first + "\": " + valueToString(it->second);
            ++it;
            if (it != dict->pairs.end()) {
                result += ", ";
            }
        }
        result += "}";
        return result;
    } else if (std::holds_alternative<std::shared_ptr<OkerInstance>>(value)) {
        auto instance = std::get<std::shared_ptr<OkerInstance>>(value);
        return instance->klass->name + " instance";
    } else if (std::holds_alternative<std::shared_ptr<OkerClass>>(value)) {
        auto klass = std::get<std::shared_ptr<OkerClass>>(value);
        return "class " + klass->name;
    }
    return "nil";
}

double VirtualMachine::valueToNumber(const Value& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value));
        } catch (...) {
            return 0.0;
        }
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    return 0.0;
}

bool VirtualMachine::valueToBoolean(const Value& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty() && std::get<std::string>(value) != "false";
    }
    return false;
}

void VirtualMachine::printStack() {
    std::cout << "Stack: ";
    std::stack<Value> temp = stack;
    std::vector<Value> values;
    while (!temp.empty()) {
        values.push_back(temp.top());
        temp.pop();
    }
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        std::cout << "[" << valueToString(*it) << "] ";
    }
    std::cout << "\n";
}

void VirtualMachine::printVariables() {
    std::cout << "Global Variables:\n";
    for (const auto& pair : globalVars) {
        std::cout << "  " << pair.first << " = " << valueToString(pair.second) << "\n";
    }

    if (!callStack.empty()) {
        std::cout << "Local Variables:\n";
        for (const auto& pair : callStack.top().localVars) {
            std::cout << "  " << pair.first << " = " << valueToString(pair.second) << "\n";
        }
    }
}