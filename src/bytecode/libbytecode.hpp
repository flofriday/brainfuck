#pragma once

#include <string>
#include <vector>

enum OpCode {
    OP_MOVE, //     1 signed byte argument to indicate moves
             //     (positive right, negative left)
    OP_INC, //      2 singend byte argument to tell by how much we increment (negative for decrement)
            //      first is the offset, second is the count
    OP_WRITE, //    no argument
    OP_READ, //     no argument
    OP_OPEN, //     8 byte argument to indicate the target position
    OP_CLOSE, //    8 byte argument to indicate the target position
    OP_CLEAR, //    no argument
    OP_MUL, //      2 singed byte arguments, first for the offset and other one for the
            //      factor
};

std::vector<uint8_t> compileByteCode(std::string source);
void printByteCode(std::vector<uint8_t> opcodes);

uint8_t readByteArgument(std::vector<uint8_t>& opcodes, uint64_t& instructionPointer);
uint64_t readEightByteArgument(std::vector<uint8_t>& opcodes, uint64_t& instructionPointer);