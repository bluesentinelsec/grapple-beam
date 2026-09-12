add_executable(grapple_chiptune_bench chiptune_bench.cpp)
target_link_libraries(grapple_chiptune_bench PRIVATE Grapple::Mixer benchmark::benchmark benchmark::benchmark_main)
target_compile_definitions(grapple_chiptune_bench PRIVATE
  CHIP_BENCH_ASSETS="${PROJECT_SOURCE_DIR}/tests/mixer/assets")
grapple_set_project_warnings(grapple_chiptune_bench)
