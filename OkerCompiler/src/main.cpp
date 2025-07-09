#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono> 
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "vm.h"
#include "optimizer.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <source_file>\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help        Show this help message\n";
    std::cout << "  -t, --tokens      Print tokens only\n";
    std::cout << "  -p, --parse       Print AST only\n";
    std::cout << "  -s, --semantic    Run semantic analysis only\n";
    std::cout << "  -b, --bytecode    Print bytecode only\n";
    std::cout << "      --time        Measure and print execution time\n"; // New option
    std::cout << "  -v, --verbose     Verbose output\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string filename;
    bool tokensOnly = false;
    bool parseOnly = false;
    bool semanticOnly = false;
    bool bytecodeOnly = false;
    bool measureTime = false; // New flag
    bool verbose = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-t" || arg == "--tokens") {
            tokensOnly = true;
        } else if (arg == "-p" || arg == "--parse") {
            parseOnly = true;
        } else if (arg == "-s" || arg == "--semantic") {
            semanticOnly = true;
        } else if (arg == "-b" || arg == "--bytecode") {
            bytecodeOnly = true;
        } else if (arg == "--time") { // Check for the new flag
            measureTime = true;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] != '-') {
            filename = arg;
        }
    }

    if (filename.empty()) {
        std::cerr << "Error: No source file specified\n";
        return 1;
    }

    // Read source file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << filename << "'\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    try {
        // Lexical analysis
        if (verbose) std::cout << "=== Lexical Analysis ===\n";
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        if (tokensOnly) {
            for (const auto& token : tokens) {
                std::cout << token.toString() << "\n";
            }
            return 0;
        }

        // Parsing
        if (verbose) std::cout << "=== Parsing ===\n";
        Parser parser(tokens);
        auto ast = parser.parse();

        if (parseOnly) {
            ast->print(0);
            return 0;
        }

        // Semantic analysis
        if (verbose) std::cout << "=== Semantic Analysis ===\n";
        SemanticAnalyzer analyzer;
        analyzer.analyze(ast.get());

        if (semanticOnly) {
            std::cout << "Semantic analysis completed successfully\n";
            return 0;
        }

        // Code generation
        if (verbose) std::cout << "=== Code Generation ===\n";
        CodeGenerator generator;
        auto bytecode = generator.generate(ast.get());
        Optimizer optimizer;
        auto optimized_bytecode = optimizer.optimize(bytecode);
        if (bytecodeOnly) {
            generator.printBytecode(bytecode);
            return 0;
        }

        // Execution
        if (verbose) std::cout << "=== Execution ===\n";

        auto startTime = std::chrono::high_resolution_clock::now();

        VirtualMachine vm;
        // vm.execute(bytecode);
        vm.execute(optimized_bytecode);

        auto endTime = std::chrono::high_resolution_clock::now();

        if (measureTime) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            double milliseconds = duration.count() / 1000.0;
            std::cout << "\n--- Execution time: " << milliseconds << " ms ---\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
