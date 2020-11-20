aux_source_directory(. _LIB_FILES)
aux_source_directory(io _LIB_IO_FILES)
add_library(lib STATIC ${_LIB_FILES} ${_LIB_IO_FILES})

target_include_directories(
  lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io>)

target_link_libraries(
  lib
  PUBLIC QuFaceIOSDK::io
  PUBLIC QuFaceSDK::face
  PUBLIC QuFaceSDK::database
  PUBLIC Qt5::Widgets
  PUBLIC zbar::zbar)

# For audio
target_link_libraries(lib PRIVATE "${HISI_SDK_PREFIX}/lib/libsecurec.so")
