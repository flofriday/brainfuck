file(GLOB H_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)

add_library(libbytecode
  STATIC
  libbytecode.hpp 
  libbytecode.cpp
)
target_include_directories(libbytecode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(
  brainbyte
  ${H_FILES}
  ${SRC_FILES}
)
set_target_properties(brainbyte PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")