#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

enum OpCode {
    OP_RIGHT, //    1 byte argument to indicate how far to the right
    OP_LEFT, //     1 byte argument to indicate how far to the left
    OP_INC, //      1 byte argument to tell by how much we increment
    OP_DEC, //      1 byte argument to tell by how much we decrement
    OP_INC_OFF, //  2 bytes arguments the first is the offset and the other the count
    OP_DEC_OFF, //  2 bytes arguments the first is the offset and the other the count
    OP_WRITE, //    no argument
    OP_READ, //     no argument
    OP_OPEN, //     8 byte argument to indicate the target position
    OP_CLOSE, //    8 byte argument to indicate the target position
    OP_CLEAR, //    no argument
    OP_COPY, //     1 singed byte arguments  for the offset
    OP_MUL, //      2 singed byte arguments one, for the offset and other one for the
            //      factor
};

struct CompileOptions {
    bool allow_OP_INC_OFF;
    bool allow_OP_DEC_OFF;
    bool allow_OP_CLEAR;
    bool allow_OP_COPY;
    bool allow_OP_MUL;
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
        case OP_RIGHT: {
            uint64_t pos = instructionPointer;
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_RIGHT " << (int)argument << std::endl;
            break;
        }

        case OP_LEFT: {
            uint64_t pos = instructionPointer;
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_LEFT " << (int)argument << std::endl;
            break;
        }

        case OP_INC: {
            uint64_t pos = instructionPointer;
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_INC " << (int)argument << std::endl;
            break;
        }

        case OP_INC_OFF: {
            uint64_t pos = instructionPointer;
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            uint8_t increment = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_INC_OFF " << (int)offset << " " << (int)increment << std::endl;
            break;
        }

        case OP_DEC: {
            uint64_t pos = instructionPointer;
            uint8_t argument = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_DEC " << (int)argument << std::endl;
            break;
        }

        case OP_DEC_OFF: {
            uint64_t pos = instructionPointer;
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            uint8_t decrement = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_DEC_OFF " << (int)offset << " " << (int)decrement << std::endl;
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

        case OP_COPY: {
            uint64_t pos = instructionPointer;
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            std::cout << std::setfill('0') << std::setw(3) << pos << ": ";
            std::cout << "OP_COPY " << (int)offset << " " << std::endl;
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
    std::map<int8_t, int16_t> offsetIncrements;

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

            if (offsetIncrements[currOffset] == UINT8_MAX) {
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

            if (offsetIncrements[currOffset] == UINT8_MAX) {
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
        int16_t increment = it.second;

        if (increment > 0) {
            emitByte(opcodes, OP_INC_OFF);
            emitByte(opcodes, offset);
            emitByte(opcodes, (uint8_t)increment);
        } else {
            emitByte(opcodes, OP_DEC_OFF);
            emitByte(opcodes, offset);
            emitByte(opcodes, (uint8_t)(-increment));
        }
    }

    // Finally write the deferred move it is not zero
    if (currOffset > 0) {
        emitByte(opcodes, OP_RIGHT);
        emitByte(opcodes, (uint8_t)(currOffset));
    } else if (currOffset < 0) {
        emitByte(opcodes, OP_LEFT);
        emitByte(opcodes, (uint8_t)(-currOffset));
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

        if (factor == 1) {
            emitByte(opcodes, OP_COPY);
            emitByte(opcodes, offset);
        } else {
            emitByte(opcodes, OP_MUL);
            emitByte(opcodes, offset);
            emitByte(opcodes, factor);
        }
    }

    // Clear the current cell and move the instruction pointer to the end of the
    // loop
    emitByte(opcodes, OP_CLEAR);
    instructionPointer = currInstructionPointer;
    return true;
}

std::vector<uint8_t> compileByteCode(std::string& source, CompileOptions& co)
{
    std::vector<uint8_t> opcodes;
    std::deque<uint64_t> jumpStack;

    bool enableDeferredMoves = co.allow_OP_INC_OFF && co.allow_OP_DEC_OFF;
    bool enableMultiplyLoop = co.allow_OP_CLEAR && co.allow_OP_COPY && co.allow_OP_MUL;

    for (uint64_t instructionPointer = 0; instructionPointer < source.size(); instructionPointer++) {
        switch (source.at(instructionPointer)) {
        case '>': {
            // Maybe, we don't need to do the move here but can defer it until
            // later.
            if (enableDeferredMoves && tryCompileDeferredMoves(source, instructionPointer, opcodes)) {
                break;
            }

            // Count how long the sequence of '>' is. (up until 255)
            uint8_t n = 1;
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '>' && n < UINT8_MAX; n++)
                ;

            // Also increase the instruction pointer accordingly.
            instructionPointer += n - 1;

            // Emit the right bytecodes
            emitByte(opcodes, OP_RIGHT);
            emitByte(opcodes, n);
            break;
        }
        case '<': {
            // Maybe, we don't need to do the move here but can defer it until
            // later.
            if (enableDeferredMoves && tryCompileDeferredMoves(source, instructionPointer, opcodes)) {
                break;
            }

            // Count how long the sequence of '<' is. (up until 255)
            uint8_t n = 1;
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '<' && n < UINT8_MAX; n++)
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
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '+' && n < UINT8_MAX; n++)
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
            for (; (instructionPointer + n) < source.size() && source.at(instructionPointer + n) == '-' && n < UINT8_MAX; n++)
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
            // First check if this is a Clear loop `[-]`
            // if (tryCompileClearLoop(source, instructionPointer, opcodes)) {
            //     break;
            // }

            // Next, it could also be a multiplication/copy loop.
            if (enableMultiplyLoop && tryCompileMultiplyLoop(source, instructionPointer, opcodes)) {
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

int main(int argc, char const* argv[])
{
    // Read input file
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT" << std::endl;
        exit(1);
    }

    std::ifstream in(argv[argc - 1]);
    std::string source(static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str());
    source = removeComments(source);

    // Setup the datastructure
    uint8_t array[30000];
    std::memset(array, 0, 30000);
    uint32_t dataPointer = 0;
    uint64_t instructionPointer = 0;
    std::unordered_map<uint64_t, uint64_t> jumpCache;

    // Compile the code to bytecode
    CompileOptions co = { false, false, true, true, true };
    auto opcodes = compileByteCode(source, co);
    if (argc > 2) {
        printByteCode(opcodes);
        std::cout << opcodes.size() << std::endl;
        exit(0);
    }

    // Interpret the bytecode
    for (; instructionPointer < opcodes.size(); instructionPointer++) {
        // std::cout << instructionPointer << " -> " << dataPointer << std::endl;
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

        case OP_CLEAR: {
            array[dataPointer] = 0;
            break;
        }

        case OP_COPY: {
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            uint64_t target = dataPointer + (int64_t)offset;
            array[target] += array[dataPointer];
            break;
        }

        case OP_MUL: {
            int8_t offset = readByteArgument(opcodes, instructionPointer);
            int8_t factor = readByteArgument(opcodes, instructionPointer);
            uint64_t target = dataPointer + (int64_t)offset;
            array[target] += array[dataPointer] * factor;
            break;
        }

            // NOTE: We disabled these instructions here because including them
            // reduces performance significantly.
            // case OP_INC_OFF: {
            //     int8_t offset = readByteArgument(opcodes, instructionPointer);
            //     uint8_t increment = readByteArgument(opcodes, instructionPointer);
            //     uint64_t target = dataPointer + (int64_t)offset;
            //     array[target] += increment;
            //     break;
            // }

            // case OP_DEC_OFF: {
            //     int8_t offset = readByteArgument(opcodes, instructionPointer);
            //     uint8_t decrement = readByteArgument(opcodes, instructionPointer);
            //     uint64_t target = dataPointer + (int64_t)offset;
            //     array[target] -= decrement;
            //     break;
            // }

        case OP_WRITE:
            std::putchar(array[dataPointer]);
            break;

        case OP_READ:
            array[dataPointer] = std::getchar();
            break;

        default:
            std::cerr << "ERROR: Unknown opcode!" << std::endl;
            exit(1);
        }
    }
}
