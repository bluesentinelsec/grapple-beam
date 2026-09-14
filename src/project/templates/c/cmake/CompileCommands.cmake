if(EXISTS "${BUILD_DIR}/compile_commands.json")
  file(COPY_FILE "${BUILD_DIR}/compile_commands.json" "${ROOT}/compile_commands.json" ONLY_IF_DIFFERENT)
endif()
