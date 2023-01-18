#include <deque>
#include <iomanip>
#include <iostream>
#include <map>

#include "libbytecode.hpp"

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

void patchByte(std::vector<uint8_t>& opcodes, uint64_t index, uint8_t byte)
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

void printByteCode(std::vector<uint8_t> opcodes)
{
    for (uint64_t instructionPointer = 0; instructionPointer < opcodes.size(); instructionPointer++) {
        switch (opcodes.at(instructionPointer)) {
        case OP_MOVE: {
            uint64_t pos = instructionPointer;
            int8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_MOVE " << (int)argument << std::endl;
            break;
        }

        case OP_INC: {
            uint64_t pos = instructionPointer;
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            int8_t n = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_INC " << (int)offset << " " << (int)n << std::endl;
            break;
        }

        case OP_WRITE: {
            uint64_t pos = instructionPointer;
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_WRITE" << std::endl;
            break;
        }

        case OP_READ: {
            uint64_t pos = instructionPointer;
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_READ" << std::endl;
            break;
        }

        case OP_OPEN: {
            uint64_t pos = instructionPointer;
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_OPEN " << argument << std::endl;
            break;
        }

        case OP_CLOSE: {
            uint64_t pos = instructionPointer;
            uint64_t argument = readEightByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_CLOSE " << argument << std::endl;
            break;
        }

        case OP_CLEAR: {
            uint64_t pos = instructionPointer;
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_CLEAR " << std::endl;
            break;
        }

        case OP_MUL: {
            uint64_t pos = instructionPointer;
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            int8_t factor = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_MUL " << (int)offset << " "
                      << (int)factor << std::endl;
            break;
        }

        default:
            std::cerr << "ERROR: Unknown opcode!" << std::endl;
            std::cerr << "InstructionPointer: " << instructionPointer << std::endl;
            std::cerr << "OpCode: " << (int)opcodes.at(instructionPointer) << std::endl;
            exit(1);
        }
    }
}

/**
 * @brief Tries to generate increment and decrement opcodes with an offset and
 * defers the actual datapointer movements until they are abosoluty necessary,
 * merging them into a single instruction.
 *
 * @param opcodes
 * @param instructionPointer
 * @return true
 * @return false
 */
bool tryCompileDeferredMoves(std::string& source, uint64_t& instructionPointer, std::vector<uint8_t>& opcodes)
{
    // NOTE: we use a singed 16 bit int here because we want to store increments
    // and decrements in the same datastructure.
    std::map<int8_t, int8_t> offsetIncrements;

    uint64_t currInstructionPointer = instructionPointer;
    int8_t currOffset = 0;
    for (bool keepLooping = true; source.size() > currInstructionPointer && keepLooping; currInstructionPointer++) {
        char ins = source.at(currInstructionPointer);
        switch (ins) {
        case '>': {
            if (currOffset == INT8_MAX) {
                keepLooping = false;
                break;
            }

            currOffset++;
            break;
        }
        case '<': {
            if (currOffset == INT8_MIN) {
                keepLooping = false;
                break;
            }

            currOffset--;
            break;
        }
        case '+': {
            if (offsetIncrements.find(currOffset) == offsetIncrements.end()) {
                offsetIncrements[currOffset] = 0;
            }

            if (offsetIncrements[currOffset] == INT8_MAX) {
                keepLooping = false;
                break;
            }

            offsetIncrements[currOffset]++;
            break;
        }
        case '-': {
            if (offsetIncrements.find(currOffset) == offsetIncrements.end()) {
                offsetIncrements[currOffset] = 0;
            }

            if (offsetIncrements[currOffset] == INT8_MAX) {
                keepLooping = false;
                break;
            }

            offsetIncrements[currOffset]--;
            break;
        }
        default:
            // This is the end of the section for which we can defer the move.
            currInstructionPointer--;
            keepLooping = false;
            break;
        }
    }

    // This optimization only makes sense if we have more than 2 increments and
    // decrements.
    char lastChar = source.at(currInstructionPointer - 1);
    if (!((offsetIncrements.size() > 1 && (lastChar == '>' || lastChar == '<')) || offsetIncrements.size() >= 2))
        return false;

    // Write all increments and decrements with the offset
    for (auto it : offsetIncrements) {
        int8_t offset = it.first;
        int8_t increment = it.second;

        if (increment == 0) {
            continue;
        }

        emitByte(opcodes, OP_INC);
        emitByte(opcodes, offset);
        emitByte(opcodes, (int8_t)increment);
    }

    // Finally write the deferred move it is not zero
    if (currOffset != 0) {
        emitByte(opcodes, OP_MOVE);
        emitByte(opcodes, (int8_t)(currOffset));
    }

    // Adjust the instruction pointer to the one before the current one cause
    // we couldn't yet generate code for the current instruction.
    instructionPointer = currInstructionPointer - 1;
    return true;
}

/**
 * @brief Tries to detect if the next couple of instructions are a clear loop
 * and if so it will emit a clear instruction.
 * You can find more about this optimization at:
 * https://github.com/lifthrasiir/esotope-bfc/wiki/Comparison#simple-loop-detection
 *
 * @param source
 * @param instructionPointer
 * @param opcodes
 * @return true if it detected and compiled a clear loop, otherwise false.
 */
bool tryCompileMultiplyLoop(std::string& source, uint64_t& instructionPointer, std::vector<uint8_t>& opcodes)
{
    // FIXME: Really thing if we could have an overflow here and how to fix it.
    std::map<int8_t, int8_t> offsetFactors;

    uint64_t currInstructionPointer = instructionPointer + 1;
    int8_t currOffset = 0;
    for (; source.at(currInstructionPointer) != ']'; currInstructionPointer++) {
        char ins = source.at(currInstructionPointer);
        switch (ins) {
        case '>': {
            if (currOffset == INT8_MAX)
                return false;

            currOffset++;
            break;
        }
        case '<': {
            if (currOffset == INT8_MIN)
                return false;

            currOffset--;
            break;
        }
        case '+': {
            if (offsetFactors.find(currOffset) == offsetFactors.end()) {
                offsetFactors[currOffset] = 0;
            }

            if (offsetFactors[currOffset] == INT8_MAX)
                return false;

            offsetFactors[currOffset]++;
            break;
        }
        case '-': {
            if (offsetFactors.find(currOffset) == offsetFactors.end()) {
                offsetFactors[currOffset] = 0;
            }

            if (offsetFactors[currOffset] == INT8_MIN)
                return false;

            offsetFactors[currOffset]--;
            break;
        }
        default:
            // This is no longer a multiply loop so just exist and fallback to
            // the general implementation of loops
            return false;
        }
    }

    // Verify that it is a multiplication loop which must have:
    // 1) An equal amount of left-right movements
    if (currOffset != 0)
        return false;

    // 2) The cell at the initial datapoint must be decremented by one.
    if (offsetFactors.find(0) == offsetFactors.end() || offsetFactors[0] != -1)
        return false;

    // Compile all multiply instructions
    for (const auto it : offsetFactors) {
        int8_t offset = it.first;
        int8_t factor = it.second;
        if (offset == 0)
            continue;

        emitByte(opcodes, OP_MUL);
        emitByte(opcodes, offset);
        emitByte(opcodes, factor);
    }

    // Clear the current cell and move the instruction pointer to the end of the
    // loop
    emitByte(opcodes, OP_CLEAR);
    instructionPointer = currInstructionPointer;
    return true;
}

std::string removeComments(std::string& source)
{
    std::string out;
    for (auto c : source) {
        switch (c) {
        case '>':
        case '<':
        case '+':
        case '-':
        case '.':
        case ',':
        case '[':
        case ']':
            out += c;
        default:
            break;
        }
    }
    return out;
}

std::vector<uint8_t> compileByteCode(std::string source)
{
    source = removeComments(source);

    std::vector<uint8_t> opcodes;
    std::deque<uint64_t> jumpStack;

    for (uint64_t instructionPointer = 0; instructionPointer < source.size(); instructionPointer++) {
        switch (source.at(instructionPointer)) {
        case '>': {
            // Maybe, we don't need to do the move here but can defer it until
            // later.
            if (tryCompileDeferredMoves(source, instructionPointer, opcodes)) {
                break;
            }

            // Count how long the sequence of '>' is. (up until 255)
            int8_t n = 1;
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '>' && n < INT8_MAX; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_MOVE);
            emitByte(opcodes, n);
            break;
        }
        case '<': {
            // Maybe, we don't need to do the move here but can defer it until
            // later.
            if (tryCompileDeferredMoves(source, instructionPointer, opcodes)) {
                break;
            }

            // Count how long the sequence of '<' is. (up until 255)
            uint8_t n = 1;
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '<' && n < INT8_MAX; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_MOVE);
            emitByte(opcodes, -n);
            break;
        }
        case '+': {
            // Count how long the sequence of '+' is. (up until 255)
            uint8_t n = 1;
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '+' && n < UINT8_MAX; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_INC);
            emitByte(opcodes, 0);
            emitByte(opcodes, n);
            break;
        }
        case '-': {
            // Count how long the sequence of '-' is. (up until 255)
            uint8_t n = 1;
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '-' && n < UINT8_MAX; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_INC);
            emitByte(opcodes, 0);
            emitByte(opcodes, -n);
            break;
        }
        case '.':
            emitByte(opcodes, OP_WRITE);
            break;
        case ',':
            emitByte(opcodes, OP_READ);
            break;
        case '[': {
            // Next, it could also be a multiplication/copy loop.
            if (tryCompileMultiplyLoop(source, instructionPointer, opcodes)) {
                break;
            }

            // Since it is not a loop we already detected let's implement a
            // the default version.
            jumpStack.push_front(opcodes.size());

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
                printByteCode(opcodes);
                exit(1);
            }

            uint64_t opening = jumpStack.at(0);
            jumpStack.pop_front();

            // Patch the opening instruction
            patchEightBytes(opcodes, opening + 1, opcodes.size() + 8);

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
