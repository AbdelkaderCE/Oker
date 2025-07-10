#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include "vm.h" // Include vm.h to get the definition of Value

class BuiltinFunctions {
private:
    std::stringstream string_builder;

public:
    Value call(const std::string& name, const std::vector<Value>& args, VirtualMachine& vm);

    // I/O Functions
    Value say(const std::vector<Value>& args, VirtualMachine& vm);
    Value input(const std::vector<Value>& args, VirtualMachine& vm);

    // Type conversion functions
    Value str(const std::vector<Value>& args, VirtualMachine& vm);
    Value num(const std::vector<Value>& args, VirtualMachine& vm);
    Value bool_func(const std::vector<Value>& args, VirtualMachine& vm);
    Value type(const std::vector<Value>& args, VirtualMachine& vm);

    // String functions
    Value len(const std::vector<Value>& args, VirtualMachine& vm);
    Value upper(const std::vector<Value>& args, VirtualMachine& vm);
    Value lower(const std::vector<Value>& args, VirtualMachine& vm);
    Value strip(const std::vector<Value>& args, VirtualMachine& vm);
    Value charAt(const std::vector<Value>& args, VirtualMachine& vm);
    Value split_str(const std::vector<Value>& args, VirtualMachine& vm);   // Renamed and declared
    Value replace_str(const std::vector<Value>& args, VirtualMachine& vm); // Renamed and declared

    // String Builder functions
    Value sbuild_new(const std::vector<Value>& args);
    Value sbuild_add(const std::vector<Value>& args, VirtualMachine& vm);
    Value sbuild_get(const std::vector<Value>& args);

    // List functions
    Value list_add(const std::vector<Value>& args); // Fixed: Removed unused vm parameter

    // Math functions
    Value abs_func(const std::vector<Value>& args, VirtualMachine& vm);
    Value random_num(const std::vector<Value>& args, VirtualMachine& vm); // Renamed and declared
    Value round_num(const std::vector<Value>& args, VirtualMachine& vm);

    // File I/O functions
    Value get(const std::vector<Value>& args, VirtualMachine& vm);
    Value save(const std::vector<Value>& args, VirtualMachine& vm);
    Value deletef(const std::vector<Value>& args, VirtualMachine& vm);
    Value exists(const std::vector<Value>& args, VirtualMachine& vm);

    // Utility functions
    Value exit_func(const std::vector<Value>& args, VirtualMachine& vm);
    Value sleep_func(const std::vector<Value>& args, VirtualMachine& vm);
};

#endif