#!/bin/bash

# Oker Compiler Build Script
# Builds the Oker compiler and sets up the development environment

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if CMake is installed
check_cmake() {
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is not installed. Please install CMake first."
        exit 1
    fi
    print_status "CMake found: $(cmake --version | head -n1)"
}

# Check if a C++ compiler is available
check_compiler() {
    if command -v g++ &> /dev/null; then
        print_status "G++ found: $(g++ --version | head -n1)"
    elif command -v clang++ &> /dev/null; then
        print_status "Clang++ found: $(clang++ --version | head -n1)"
    else
        print_error "No C++ compiler found. Please install g++ or clang++."
        exit 1
    fi
}

# Check if Python is available for web server
check_python() {
    if command -v python3 &> /dev/null; then
        print_status "Python3 found: $(python3 --version)"
    else
        print_warning "Python3 not found. Web server will not be available."
    fi
}

# Create build directory
create_build_dir() {
    print_status "Creating build directory..."
    mkdir -p build
    cd build
}

# Configure with CMake
configure_cmake() {
    print_status "Configuring with CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
}

# Build the project
build_project() {
    print_status "Building the project..."
    make -j$(nproc 2>/dev/null || echo 4)
}

# Run tests
run_tests() {
    print_status "Running tests..."
    if make test; then
        print_success "All tests passed!"
    else
        print_warning "Some tests failed. Check the output above."
    fi
}

# Install the binary
install_binary() {
    print_status "Installing binary..."
    cp oker ..
    cd ..
    chmod +x oker
    print_success "Oker compiler installed successfully!"
}

# Create example programs
create_examples() {
    print_status "Example programs are available in the examples/ directory:"
    if [ -d "examples" ]; then
        ls examples/*.oker | sed 's/^/  - /'
    fi
}

# Set up web server
setup_web_server() {
    if command -v python3 &> /dev/null; then
        print_status "Web server is available. Run './web/server.py' to start it."
    fi
}

# Main build function
main() {
    print_status "Starting Oker Compiler build process..."
    
    # Check prerequisites
    check_cmake
    check_compiler
    check_python
    
    # Build process
    create_build_dir
    configure_cmake
    build_project
    run_tests
    install_binary
    
    # Post-build setup
    create_examples
    setup_web_server
    
    print_success "Build completed successfully!"
    echo
    print_status "Usage:"
    echo "  ./oker program.oker              # Compile and run"
    echo "  ./oker -t program.oker           # Show tokens"
    echo "  ./oker -p program.oker           # Show AST"
    echo "  ./oker -b program.oker           # Show bytecode"
    echo "  ./oker --time program.oker       # Measure execution time"
    echo "  ./oker -h                        # Show help"
    echo
    print_status "Try running an example:"
    echo "  ./oker examples/hello.oker"
    echo
    if command -v python3 &> /dev/null; then
        print_status "Start the web interface:"
        echo "  cd web && python3 server.py"
        echo "  Then open http://localhost:5000 in your browser"
    fi
}

# Handle command line arguments
case "${1:-}" in
    -h|--help)
        echo "Oker Compiler Build Script"
        echo
        echo "Usage: $0 [OPTIONS]"
        echo
        echo "Options:"
        echo "  -h, --help     Show this help message"
        echo "  -c, --clean    Clean build directory"
        echo "  -d, --debug    Build in debug mode"
        echo "  -t, --test     Run tests only"
        echo
        exit 0
        ;;
    -c|--clean)
        print_status "Cleaning build directory..."
        rm -rf build
        rm -f oker
        print_success "Build directory cleaned!"
        exit 0
        ;;
    -d|--debug)
        print_status "Building in debug mode..."
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Debug
        make -j$(nproc 2>/dev/null || echo 4)
        cp oker ..
        cd ..
        print_success "Debug build completed!"
        exit 0
        ;;
    -t|--test)
        if [ ! -f "build/test_oker" ]; then
            print_error "Tests not built. Run build script first."
            exit 1
        fi
        print_status "Running tests..."
        cd build
        ./test_oker
        exit 0
        ;;
    "")
        main
        ;;
    *)
        print_error "Unknown option: $1"
        echo "Use -h or --help for usage information."
        exit 1
        ;;
esac
