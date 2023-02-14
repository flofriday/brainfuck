# llvm

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# My jit compiler
file(GLOB H_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)

add_executable(
    brainllvm
    ${H_FILES}
    ${SRC_FILES}
)

llvm_map_components_to_libnames(llvm_libs support core irreader)
set_target_properties(brainllvm PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
target_link_libraries(brainllvm libbytecode ${llvm_libs})