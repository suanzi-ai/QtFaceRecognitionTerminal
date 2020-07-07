aux_source_directory(. _APP_FILES)
add_library(app STATIC ${_APP_FILES})

target_include_directories(
  app PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/app>)

target_link_libraries(app PUBLIC lib service)
