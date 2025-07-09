cmake_minimum_required(VERSION 3.16)
project(OkerCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Source files
set(SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/semantic.cpp
    src/codegen.cpp
    src/optimizer.cpp
    src/vm.cpp
    src/builtins.cpp
)

# Headers
set(HEADERS
    src/lexer.h
    src/parser.h
    src/semantic.h
    src/codegen.h
    src/optimizer.h
    src/vm.h
    src/builtins.h
)

# Main executable
add_executable(oker src/main.cpp ${SOURCES} ${HEADERS})

# --- Testing ---
# FIX: Create a separate executable for each test file to avoid "multiple definition"
# errors from the linker. This assumes your test files are in a 'tests/' directory.
enable_testing()

# You can add your test files here one by one.
# Example for a hypothetical test_lexer.cpp:
# if(EXISTS "${CMAKE_SOURCE_DIR}/tests/test_lexer.cpp")
#     add_executable(test_lexer tests/test_lexer.cpp src/lexer.cpp)
#     target_include_directories(test_lexer PRIVATE src)
#     add_test(NAME LexerTests COMMAND test_lexer)
# endif()

# Example for a hypothetical test_parser.cpp:
# if(EXISTS "${CMAKE_SOURCE_DIR}/tests/test_parser.cpp")
#     add_executable(test_parser tests/test_parser.cpp src/parser.cpp src/lexer.cpp)
#     target_include_directories(test_parser PRIVATE src)
#     add_test(NAME ParserTests COMMAND test_parser)
# endif()

# Include directories
target_include_directories(oker PRIVATE src)
