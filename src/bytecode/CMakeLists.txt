file(GLOB H_FILES *.hpp)
file(GLOB SRC_FILES *.cpp)

add_executable(
  brainbyte
  ${H_FILES}
  ${SRC_FILES}
)

set_target_properties(brainbyte PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")