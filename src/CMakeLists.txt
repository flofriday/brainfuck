add_subdirectory(interpreter)
add_subdirectory(bytecode)

# dynasm can only run and only be build on x86 or x86_64
message(STATUS ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86" OR ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    add_subdirectory(dynasm)
endif()

#add_subdirectory(llvm)