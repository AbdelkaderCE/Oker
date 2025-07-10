#include "builtins.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cctype>

Value BuiltinFunctions::call(const std::string& name, const std::vector<Value>& args, VirtualMachine& vm) {
    if (name == "say") return say(args, vm);
    if (name == "input") return input(args, vm);
    if (name == "str") return str(args, vm);
    if (name == "num") return num(args, vm);
    if (name == "bool") return bool_func(args, vm);
    if (name == "type") return type(args, vm);
    if (name == "len") return len(args, vm);
    if (name == "upper") return upper(args, vm);
    if (name == "lower") return lower(args, vm);
    if (name == "strip") return strip(args, vm);
    if (name == "charAt") return charAt(args, vm);
    if (name == "sbuild_new") return sbuild_new(args);
    if (name == "sbuild_add") return sbuild_add(args, vm);
    if (name == "sbuild_get") return sbuild_get(args);
    if (name == "abs") return abs_func(args, vm);
    if (name == "get") return get(args, vm);
    if (name == "save") return save(args, vm);
    if (name == "deletef") return deletef(args, vm);
    if (name == "exists") return exists(args, vm);
    if (name == "exit") return exit_func(args, vm);
    if (name == "sleep") return sleep_func(args, vm);
    if (name == "list_add") return list_add(args, vm);

    throw std::runtime_error("Unknown built-in function: " + name);
}

// I/O Functions
Value BuiltinFunctions::say(const std::vector<Value>& args, VirtualMachine& vm) {
    for (size_t i = 0; i < args.size(); i++) {
        std::cout << vm.valueToString(args[i]);
        if (i < args.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
    return Value(0.0);
}

Value BuiltinFunctions::input(const std::vector<Value>& args, VirtualMachine& vm) {
    if (!args.empty()) {
        std::cout << vm.valueToString(args[0]);
    }
    std::string line;
    std::getline(std::cin, line);
    return Value(line);
}

// Type conversion functions
Value BuiltinFunctions::str(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(std::string(""));
    return Value(vm.valueToString(args[0]));
}

Value BuiltinFunctions::num(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(0.0);
    return Value(vm.valueToNumber(args[0]));
}

Value BuiltinFunctions::bool_func(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(false);
    return Value(vm.valueToBoolean(args[0]));
}

Value BuiltinFunctions::type(const std::vector<Value>& args, VirtualMachine& vm) {
    (void)vm; // Suppress unused parameter warning
    if (args.empty()) return Value(std::string("void"));

    const Value& value = args[0];
    if (std::holds_alternative<double>(value)) {
        return Value(std::string("number"));
    } else if (std::holds_alternative<std::string>(value)) {
        return Value(std::string("string"));
    } else if (std::holds_alternative<bool>(value)) {
        return Value(std::string("boolean"));
    } else if (std::holds_alternative<std::shared_ptr<OkerList>>(value)) {
        return Value(std::string("list"));
    }
    return Value(std::string("unknown"));
}

// String functions
Value BuiltinFunctions::len(const std::vector<Value>& args, VirtualMachine& vm) {
    (void)vm; // Suppress unused parameter warning
    if (args.empty()) return Value(0.0);

    const Value& val = args[0];
    if (std::holds_alternative<std::string>(val)) {
        return Value(static_cast<double>(std::get<std::string>(val).length()));
    }
    if (std::holds_alternative<std::shared_ptr<OkerList>>(val)) {
        return Value(static_cast<double>(std::get<std::shared_ptr<OkerList>>(val)->elements.size()));
    }
    return Value(0.0);
}

Value BuiltinFunctions::upper(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(std::string(""));
    std::string str = vm.valueToString(args[0]);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return Value(str);
}

Value BuiltinFunctions::lower(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(std::string(""));
    std::string str = vm.valueToString(args[0]);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return Value(str);
}

Value BuiltinFunctions::strip(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(std::string(""));
    std::string str = vm.valueToString(args[0]);

    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());

    return Value(str);
}

Value BuiltinFunctions::charAt(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.size() < 2) return Value(std::string(""));

    std::string str = vm.valueToString(args[0]);
    int index = static_cast<int>(vm.valueToNumber(args[1]));

    if (index < 0 || index >= static_cast<int>(str.length())) {
        return Value(std::string(""));
    }

    return Value(std::string(1, str[index]));
}

// String Builder functions
Value BuiltinFunctions::sbuild_new(const std::vector<Value>& args) {
    (void)args; // Suppress unused parameter warning
    string_builder.str("");
    string_builder.clear();
    return Value(true);
}

Value BuiltinFunctions::sbuild_add(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(false);
    string_builder << vm.valueToString(args[0]);
    return Value(true);
}

Value BuiltinFunctions::sbuild_get(const std::vector<Value>& args) {
    (void)args; // Suppress unused parameter warning
    return Value(string_builder.str());
}


// Math functions
Value BuiltinFunctions::abs_func(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(0.0);
    return Value(std::abs(vm.valueToNumber(args[0])));
}

// File I/O functions
Value BuiltinFunctions::get(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(false);

    std::string filename = vm.valueToString(args[0]);
    std::ifstream file(filename);

    if (!file.is_open()) {
        return Value(false);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    return Value(content);
}

Value BuiltinFunctions::save(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.size() < 2) return Value(false);

    std::string filename = vm.valueToString(args[0]);
    std::string content = vm.valueToString(args[1]);

    std::ofstream file(filename);
    if (!file.is_open()) {
        return Value(false);
    }

    file << content;
    file.flush();
    file.close();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return Value(true);
}

Value BuiltinFunctions::deletef(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(false);

    std::string filename = vm.valueToString(args[0]);

    try {
        if (std::filesystem::exists(filename)) {
            return Value(std::filesystem::remove(filename));
        }
        return Value(false);
    } catch (...) {
        return Value(false);
    }
}

Value BuiltinFunctions::exists(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(false);

    std::string filename = vm.valueToString(args[0]);
    return Value(std::filesystem::exists(filename));
}

// Utility functions
Value BuiltinFunctions::exit_func(const std::vector<Value>& args, VirtualMachine& vm) {
    int code = args.empty() ? 0 : static_cast<int>(vm.valueToNumber(args[0]));
    std::exit(code);
    return Value(0.0);
}

Value BuiltinFunctions::sleep_func(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.empty()) return Value(0.0);

    double seconds = vm.valueToNumber(args[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
    return Value(0.0);
}
// List functions
Value BuiltinFunctions::list_add(const std::vector<Value>& args, VirtualMachine& vm) {
    if (args.size() < 2) {
        throw std::runtime_error("list_add expects a list and a value to add");
    }

    const auto& list_val = args[0];
    const auto& new_element = args[1];

    if (!std::holds_alternative<std::shared_ptr<OkerList>>(list_val)) {
        throw std::runtime_error("First argument to list_add must be a list");
    }

    auto list = std::get<std::shared_ptr<OkerList>>(list_val);
    list->elements.push_back(new_element);

    return list_val;
}
