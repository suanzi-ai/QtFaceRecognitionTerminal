aux_source_directory(. _APP_FILES)
aux_source_directory(./temperature_app _TEMPERATURE_APP_FILES)

add_library(app STATIC ${_APP_FILES} ${_TEMPERATURE_APP_FILES})

target_include_directories(
  app PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/app>
             ${PROJECT_SOURCE_DIR}/src/app/temperature_app)

target_link_libraries(
  app
  PUBLIC lib
  PUBLIC service)
