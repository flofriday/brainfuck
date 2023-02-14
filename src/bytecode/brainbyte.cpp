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

    // Setup the datastructure
    uint8_t array[30000];
    std::memset(array, 0, 30000);
    uint8_t* dataPointer = array;
    uint64_t instructionPointer = 0;

    // Compile the code to bytecode
    auto opcodes = compileByteCode(source);
    if (argc > 2) {
        printByteCode(opcodes);
        std::cout << opcodes.size() << std::endl;
        exit(0);
    }

    // Interpret the bytecode
    for (; instructionPointer < opcodes.size(); instructionPointer++) {
        // std::cout << instructionPointer << " -> " << dataPointer << std::endl;
        switch (opcodes.at(instructionPointer)) {
        case OP_MOVE: {
            int8_t argument = readByteArgument(opcodes, instructionPointer);
            dataPointer += argument;
            break;
        }

        case OP_INC: {
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            int8_t increment = readByteArgument(opcodes, instructionPointer);
            *(dataPointer + offset) += increment;
            break;
        }

        case OP_OPEN: {
            // If the byte at the datapointer is not zero we don't do anything
            if (*dataPointer != 0) {
                // jump over argument
                instructionPointer += 8;
                break;
            }

            // Jump to the target destination
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            instructionPointer = argument;
            break;
        }

        case OP_CLOSE: {
            // If the byte at the datapointer is zero we don't do anything
            if (*dataPointer == 0) {
                // jump over argument
                instructionPointer += 8;
                break;
            }

            // Jump to the target destination
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            instructionPointer = argument;
            break;
        }

        case OP_CLEAR: {
            *dataPointer = 0;
            break;
        }

        case OP_MUL: {
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            int8_t factor = readByteArgument(opcodes, instructionPointer);
            *(dataPointer + offset) += *dataPointer * factor;
            break;
        }

        case OP_WRITE:
            std::putchar(*dataPointer);
            break;

        case OP_READ:
            *dataPointer = std::getchar();
            break;

        default:
            std::cerr << "ERROR: Unknown opcode!" << std::endl;
            exit(1);
        }
    }
}
