aux_source_directory(. _LIB_FILES)
aux_source_directory(io _LIB_IO_FILES)
aux_source_directory(audio _LIB_AUDIO_FILES)
add_library(lib STATIC ${_LIB_FILES} ${_LIB_IO_FILES} ${_LIB_AUDIO_FILES})

target_include_directories(
  lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/audio>)

target_link_libraries(
  lib
  PUBLIC spdlog::spdlog
  PUBLIC ${HiSiSDK_LIBS}
  PUBLIC ${OpenCV_LIBS}
  PUBLIC QuFaceSDK::face
  PUBLIC QuFaceSDK::database
  PUBLIC Qt5::Widgets)

# For audio
target_link_libraries(lib PRIVATE "${HISI_SDK_PREFIX}/lib/libsecurec.so")
