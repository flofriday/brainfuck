#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

enum OpCode {
    OP_RIGHT, //    1 byte argument to indicate how far to the right
    OP_LEFT, //     1 byte argument to indicate how far to the left
    OP_INC, //      1 byte argument to tell by how much we increment
    OP_DEC, //      1 byte argument to tell by how much we decrement
    OP_WRITE, //    no argument
    OP_READ, //     no argument
    OP_OPEN, //     8 byte argument to indicate the target position
    OP_CLOSE, //    8 byte argument to indicate the target position
};

void emitByte(std::vector<uint8_t>& opcodes, uint8_t byte)
{
    opcodes.push_back(byte);
}

void emitEightBytes(std::vector<uint8_t>& opcodes, uint64_t bytes)
{
    for (int i = 7; i >= 0; i--) {
        uint8_t byte = (bytes & (0xff << i * 8)) >> i * 8;
        emitByte(opcodes, byte);
    }
}

void patchByte(std::vector<uint8_t>& opcodes, uint8_t index, uint8_t byte)
{
    opcodes[index] = byte;
}

void patchEightBytes(std::vector<uint8_t>& opcodes, uint64_t index, uint64_t bytes)
{
    for (int i = 7; i >= 0; i--) {
        uint8_t byte = (bytes & (0xff << i * 8)) >> i * 8;
        patchByte(opcodes, index + (7 - i), byte);
    }

    for (size_t i = 0; i < 8; i++) {
        uint8_t byte = bytes & 0xff;
        bytes = bytes >> 8;
        patchByte(opcodes, index + (7 - i), byte);
    }
}

uint8_t readByteArgument(std::vector<uint8_t>& opcodes, uint64_t& instructionPointer)
{
    instructionPointer++;
    return opcodes.at(instructionPointer);
}

uint64_t readEightByteArgument(std::vector<uint8_t>& opcodes, uint64_t& instructionPointer)
{
    uint64_t out = 0;
    for (size_t i = 0; i < 8; i++) {
        out = out << 8;
        out += readByteArgument(opcodes, instructionPointer);
    }
    return out;
}

std::vector<uint8_t> compileByteCode(std::string& source)
{
    std::vector<uint8_t> opcodes;
    std::deque<uint64_t> jumpStack;

    for (uint64_t instructionPointer = 0; instructionPointer < source.size(); instructionPointer++) {
        switch (source.at(instructionPointer)) {
        case '>': {
            // Count how long the sequence of '>' is. (up until 255)
            uint8_t n = 1;
            for (; source.at(instructionPointer + n) == '>' && n < 255; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_RIGHT);
            emitByte(opcodes, n);
            break;
        }
        case '<': {
            // Count how long the sequence of '<' is. (up until 255)
            uint8_t n = 1;
            for (; source.at(instructionPointer + n) == '<' && n < 255; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_LEFT);
            emitByte(opcodes, n);
            break;
        }
        case '+': {
            // Count how long the sequence of '+' is. (up until 255)
            uint8_t n = 1;
            for (; source.at(instructionPointer + n) == '+' && n < 255; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_INC);
            emitByte(opcodes, n);
            break;
        }
        case '-': {
            // Count how long the sequence of '-' is. (up until 255)
            uint8_t n = 1;
            for (; source.at(instructionPointer + n) == '-' && n < 255; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_DEC);
            emitByte(opcodes, n);
            break;
        }
        case '.':
            emitByte(opcodes, OP_WRITE);
            break;
        case ',':
            emitByte(opcodes, OP_READ);
            break;
        case '[': {
            jumpStack.push_back(opcodes.size());

            // Emit the bytecode to a open jump and an invalid jump target that
            // we will patch later when we find the matching closing bracket.
            emitByte(opcodes, OP_OPEN);
            emitEightBytes(opcodes, 0x0);
            break;
        }
        case ']': {
            // Load the matching opening
            if (jumpStack.empty()) {
                std::cerr << "Error: Couldn't find matching '['" << std::endl;
                exit(1);
            }

            uint64_t opening = jumpStack.at(0);
            jumpStack.pop_front();

            // Patch the opening instruction
            patchEightBytes(opcodes, opening + 1, opcodes.size() + 9); // FIXME: why is it 9???

            // Emit Opcodes for closing
            emitByte(opcodes, OP_CLOSE);
            emitEightBytes(opcodes, opening + 8);
            break;
        }
        default:
            break;
        }
    }

    if (!jumpStack.empty()) {
        std::cerr << "Error: Couldn't find matching ']'" << std::endl;
        exit(1);
    }

    return opcodes;
}

void printByteCode(std::vector<uint8_t> opcodes)
{
    for (uint64_t instructionPointer = 0; instructionPointer < opcodes.size(); instructionPointer++) {
        switch (opcodes.at(instructionPointer)) {
        case OP_RIGHT: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << "OP_RIGHT " << (int)argument << std::endl;
            break;
        }

        case OP_LEFT: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << "OP_LEFT " << (int)argument << std::endl;
            break;
        }

        case OP_INC: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << "OP_INC " << (int)argument << std::endl;
            break;
        }

        case OP_DEC: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << "OP_DEC " << (int)argument << std::endl;
            break;
        }

        case OP_WRITE:
            std::cout << "OP_WRITE" << std::endl;
            break;

        case OP_READ:
            std::cout << "OP_READ" << std::endl;
            break;

        case OP_OPEN: {
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            std::cout << "OP_OPEN " << argument << std::endl;
            break;
        }

        case OP_CLOSE: {
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            std::cout << "OP_CLOSE " << argument << std::endl;
            break;
        }

        default:
            std::cerr << "ERROR: Unknown opcode!" << std::endl;
            std::cerr << (int)opcodes.at(instructionPointer) << std::endl;
            exit(1);
        }
    }
}

int main(int argc, char const* argv[])
{
    // Read input file
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT" << std::endl;
        exit(1);
    }

    std::ifstream in(argv[1]);
    std::string source(static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str());

    // Setup the datastructure
    uint8_t array[30000];
    std::memset(array, 0, 30000);
    uint32_t dataPointer = 0;
    uint64_t instructionPointer = 0;
    std::unordered_map<uint64_t, uint64_t> jumpCache;

    // Compile the code to bytecode
    auto opcodes = compileByteCode(source);
    // printByteCode(opcodes);

    // Interpret the bytecode
    for (; instructionPointer < opcodes.size(); instructionPointer++) {
        switch (opcodes.at(instructionPointer)) {
        case OP_RIGHT: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            dataPointer += argument;
            break;
        }

        case OP_LEFT: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            dataPointer -= argument;
            break;
        }

        case OP_INC: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            array[dataPointer] += argument;
            break;
        }

        case OP_DEC: {
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            array[dataPointer] -= argument;
            break;
        }

        case OP_WRITE:
            std::putchar(array[dataPointer]);
            break;

        case OP_READ:
            array[dataPointer] = std::getchar();
            break;

        case OP_OPEN: {
            // If the byte at the datapointer is not zero we don't do anything
            if (array[dataPointer] != 0) {
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
            if (array[dataPointer] == 0) {
                // jump over argument
                instructionPointer += 8;
                break;
            }

            // Jump to the target destination
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            instructionPointer = argument;
            break;
        }

        default:
            std::cerr << "ERROR: Unknown opcode!" << std::endl;
            exit(1);
        }
    }
}
