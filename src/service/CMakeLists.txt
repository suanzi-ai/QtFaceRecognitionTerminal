aux_source_directory(. _SERVICE_FILES)

add_library(service ${_SERVICE_FILES})

target_include_directories(
  service PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(
  service
  PUBLIC lib
  PUBLIC app
  PUBLIC httplib::httplib
  PUBLIC QuFaceSDK::face
  PUBLIC QuFaceSDK::database)
