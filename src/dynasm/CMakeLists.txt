file(GLOB H_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)

add_executable(
  braindyn
  ${H_FILES}
  ${SRC_FILES}
)

set_target_properties(braindyn PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
target_link_libraries(braindyn libbytecode)