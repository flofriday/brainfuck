set(CMAKE_C_COMPILER "clang")
set(MINILUA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT/src/host/minilua.c")
add_executable(minilua ${MINILUA_SRC})
target_link_libraries(minilua m)

set_source_files_properties(${MINILUA_SRC} PROPERTIES LANGUAGE C)
set_target_properties(minilua PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Create a custom command to run first with second.txt
#./minilua luajit-2.0/dynasm/dynasm.lua -o tutorial.posix64.c -D X64 tutorial.c
add_custom_command(
    OUTPUT braindyn.cpp
    DEPENDS minilua braindyn.cpp.dyn
    COMMAND minilua "${CMAKE_CURRENT_SOURCE_DIR}/LuaJIT/dynasm/dynasm.lua" -o braindyn.cpp -D X64 "${CMAKE_CURRENT_SOURCE_DIR}/braindyn.cpp.dyn"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# file(GLOB H_FILES *.hpp)
# file(GLOB SRC_FILES *.cpp)
file(COPY LuaJIT DESTINATION ${CMAKE_CURRENT_BINARY_DIR} 
    PATTERN "*" 
)

add_executable(
    braindyn
    braindyn.cpp
)

set_target_properties(braindyn PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
target_link_libraries(braindyn libbytecode)