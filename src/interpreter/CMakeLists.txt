file(GLOB H_FILES *.h)
file(GLOB SRC_FILES *.cpp)

add_executable(
  brainint
  ${H_FILES}
  ${SRC_FILES}
)

set_target_properties(brainint PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")