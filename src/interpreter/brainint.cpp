#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

    // Program loop
    for (; instructionPointer < source.size(); instructionPointer++) {
        switch (source.at(instructionPointer)) {
        case '>':
            dataPointer++;
            break;
        case '<':
            dataPointer--;
            break;
        case '+':
            array[dataPointer]++;
            break;
        case '-':
            array[dataPointer]--;
            break;
        case '.':
            std::putchar(array[dataPointer]);
            break;
        case ',':
            array[dataPointer] = std::getchar();
            break;
        case '[': {
            // If the byte at the datapointer is not zero we don't do anything
            if (array[dataPointer] != 0) {
                break;
            }

            // FIXME: We should cache the targets position in a map instead of
            // always calculating it.
            // If it is zero we jump forward to the next matching ']'
            uint32_t nesting = 1;
            while (true) {
                // Do some bound checking
                if (instructionPointer >= source.size() - 1) {
                    std::cerr << "Error: Couldn't find matching ']'" << std::endl;
                    exit(1);
                }

                // Increment the instruction pointer, maybe the next is the one
                // we searched for.
                instructionPointer++;

                // Calculate the nesting
                if (source.at(instructionPointer) == '[') {
                    nesting++;
                } else if (source.at(instructionPointer) == ']') {
                    nesting--;
                }

                // Exit if the match was found
                if (nesting == 0) {
                    break;
                }
            }

            break;
        }
        case ']': {
            // If the byte at the datapointer is zero we don't do anything
            if (array[dataPointer] == 0) {
                break;
            }

            // FIXME: We should cache the targets position in a map instead of
            // always calculating it.
            // Otherwise we go back until the matching '['
            uint32_t nesting = 1;
            while (true) {
                // Do some bound checking
                if (instructionPointer == 0) {
                    std::cerr << "Error: Couldn't find matching ']'" << std::endl;
                    exit(1);
                }

                // Let's look at the previous character, maybe it's the one
                // we were looking for.
                instructionPointer--;

                // Calculate the nesting
                if (source[instructionPointer] == ']') {
                    nesting++;
                } else if (source[instructionPointer] == '[') {
                    nesting--;
                }

                // Exit if we found the match
                if (nesting == 0)
                    break;
            }

            break;
        }
        default:
            // Everything that is not a brainfuck character is treaded as a
            // comment and ignored.
            break;
        }
    }

    return 0;
}
