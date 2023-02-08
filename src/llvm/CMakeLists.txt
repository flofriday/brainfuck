# llvm
set(CMAKE_BUILD_TYPE Debug)
set(LLVM_INCLUDE_BENCHMARKS OFF)
set(LLVM_INCLUDE_EXAMPLES OFF)
set(LLVM_INCLUDE_TESTS OFF)
set(LLVM_INCLUDE_TOOLS OFF)

add_subdirectory(llvmproject/llvm)
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)


# My jit compiler
file(GLOB H_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)

add_executable(
    brainllvm
    ${H_FILES}
    ${SRC_FILES}
)

set_target_properties(brainllvm PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
target_link_libraries(brainllvm libbytecode ${llvm_libs})