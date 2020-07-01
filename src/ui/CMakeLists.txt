aux_source_directory(. _UI_FILES)
add_library(ui STATIC ${_UI_FILES})

target_include_directories(
  ui PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/ui>)

target_link_libraries(ui PUBLIC app)
