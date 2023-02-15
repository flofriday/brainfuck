#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "libbytecode.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

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
    auto TheContext = std::make_unique<llvm::LLVMContext>();
    auto TheModule = std::make_unique<llvm::Module>("brainllvm jit", *TheContext);
    auto Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

    // Build a basic function entry into which the whole brainfuck code gets
    // compiled
    llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), false);
    std::string Name = "bf_main";
    llvm::Function* TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule.get());

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    // Create the tape
    // Builder->CreateAlloca(llvm::ArrayType::getInt8Ty(*TheContext));
    Builder->CreateAlloca(llvm::ArrayType::get(llvm::ArrayType::getInt8Ty(*TheContext), 30000));

    // auto v = llvm::ConstantInt(llvm::IntegerType::getInt64Ty(*TheContext), llvm::APInt((uint64_t)0));

    Builder->CreateRet(nullptr);
    llvm::verifyFunction(*TheFunction);
    TheFunction->print(llvm::errs());
    return 1;

    for (uint64_t i = 0; i < opcodes.size(); i++) {
        // std::cout << instructionPointer << " -> " << dataPointer << std::endl;
        switch (opcodes.at(i)) {
        case OP_MOVE: {
            int8_t argument = readByteArgument(opcodes, i);
            // Builder->CreateAdd();
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
