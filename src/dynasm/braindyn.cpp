#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <libbytecode.hpp>

int main(int argc, char const* argv[])
{
    // Read input file
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT" << std::endl;
        exit(1);
    }

    std::ifstream in(argv[argc - 1]);
    std::string source(static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str());

    // Compile the code to bytecode
    auto opcodes = compileByteCode(source);
    if (argc > 2) {
        printByteCode(opcodes);
        std::cout << opcodes.size() << std::endl;
        exit(0);
    }

    // Compile to machine code
    // FIXME: Implement

    // Setup the datastructure
    uint8_t array[30000];
    std::memset(array, 0, 30000);
    uint32_t dataPointer = 0;
    uint64_t instructionPointer = 0;
    std::unordered_map<uint64_t, uint64_t> jumpCache;

    // Run native code
    // FIXME: Implement
}