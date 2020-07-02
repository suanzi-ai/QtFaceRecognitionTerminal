aux_source_directory(. _SERVER_FILES)

add_library(server ${_SERVER_FILES})

target_include_directories(
  server PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(
  server
  PUBLIC lib
  PUBLIC CURL::libcurl
  PUBLIC httplib::httplib
  PUBLIC QuFaceSDK::face
  PUBLIC QuFaceSDK::database)
