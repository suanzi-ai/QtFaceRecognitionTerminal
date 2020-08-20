aux_source_directory(. _APP_FILES)
aux_source_directory(./temperature _TEMPERATURE_FILES)

add_library(app STATIC ${_APP_FILES} ${_TEMPERATURE_FILES})


target_include_directories(
  app
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/app>
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/app/temperature>
  )

target_link_libraries(app PUBLIC lib service temperature)
