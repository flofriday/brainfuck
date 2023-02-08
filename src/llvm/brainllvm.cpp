#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "libbytecode.hpp"

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

    // Compile to llvm IR
    for (uint64_t i = 0; i < opcodes.size(); i++) {
        // std::cout << instructionPointer << " -> " << dataPointer << std::endl;
        switch (opcodes.at(i)) {
        case OP_MOVE: {
            int8_t argument = readByteArgument(opcodes, i);
            break;
        }

        case OP_INC: {
            int8_t offset = readByteArgument(opcodes, i);
            int8_t increment = readByteArgument(opcodes, i);
            break;
        }

        case OP_OPEN: {
            uint64_t argument = readEightByteArgument(opcodes, i);
            break;
        }

        case OP_CLOSE: {
            uint64_t argument = readEightByteArgument(opcodes, i);
            break;
        }

        case OP_CLEAR: {
            break;
        }

        case OP_MUL: {
            int8_t offset = readByteArgument(opcodes, i);
            int8_t factor = readByteArgument(opcodes, i);
            break;
        }

        case OP_WRITE:
            break;

        case OP_READ:
            break;

        default:
            std::cerr << "ERROR: Unknown opcode!" << std::endl;
            exit(1);
        }
    }

    // Setup the datastructure
    uint8_t array[30000];
    std::memset(array, 0, 30000);

    // Run the compiled function.
}
