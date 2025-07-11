# Oker Programming Language

Oker is a custom-built, interpreted programming language designed with a focus on human-readable syntax and a rich set of modern language features. It compiles to a custom bytecode format that is executed by a lightweight virtual machine.

This repository contains the full source code for the C++ compiler, the virtual machine, and a web-based IDE for an interactive coding experience.

---

### Current Status: Partially Functional



## Features

Oker is designed to be intuitive and powerful, providing a range of features for general-purpose programming.

### Core Language

- **Intuitive Syntax:** Commands are designed to be self-explanatory (e.g., `say` for printing).
- **Data Types:** Supports numbers, strings, booleans, lists, and dictionaries.
- **Data Structures:** First-class support for lists (`[]`) and dictionaries (`{}`), including nested structures.
- **Control Flow:** Full support for `if/else` conditionals and `while/repeat` loops, with `break` and `continue` statements.
- **Functions:** Define custom functions with parameters and return values using the `makef` keyword.
- **Error Handling:** Built-in `try/fail` blocks for robust exception management.
- **Comments:** Supports single-line (`~`) and multi-line (`~~`) comments.

### Standard Library

A rich set of built-in functions is available out-of-the-box:

- **I/O:** `say`, `input`
- **Type Conversion:** `str`, `num`, `bool`
- **String Manipulation:** `len`, `upper`, `lower`, `strip`, `split_str`, `replace_str`
- **Math:** `abs`, `max`, `min`, `round`, `sqrt`, `pow`, `random`
- **File System:** `save`, `get`, `deletef`, `exists`
- **Utility:** `exit`, `sleep`

---

## Compiler & Tooling

- **Bytecode VM:** Compiles to custom bytecode for efficient execution on a lightweight virtual machine.
- **Optimization:** Includes a peephole optimizer for simple performance improvements like converting `x = x + 1` to a single `INCREMENT` instruction.
- **Web IDE:** An interactive, browser-based editor for a seamless development experience.

---

## Syntax at a Glance

Here is a comprehensive example demonstrating the key syntax features of Oker:

```oker
~ Comprehensive Oker Language Feature Test

say "--- Oker Language Comprehensive Test ---"

~ Variables and operations
let message = "Hello, Oker!"
let a = 42
let b = 10
say message + " The sum of a+b is: " + str(a + b)

~ Functions
makef greet(name):
    return "Hello, " + name + "!"
end
say greet("World")

~ Loops and Conditionals
let i = 0
while i < 3:
    if i % 2 == 0:
        say str(i) + " is even."
    else:
        say str(i) + " is odd."
    end
    let i = i + 1
end

~ Data Structures
let my_list = [1, "two", {"key": "value"}]
my_list[1] = "zwei" ~ Modify list
say "List item: " + my_list[1]
say "Nested dictionary value: " + my_list[2]["key"]

~ Error Handling
try:
    say "Attempting risky operation..."
    let result = 10 / 0
fail:
    say "Caught an error as expected!"
end
