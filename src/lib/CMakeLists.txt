aux_source_directory(. _LIB_FILES)
aux_source_directory(io _LIB_IO_FILES)
add_library(lib STATIC ${_LIB_FILES} ${_LIB_IO_FILES})

target_include_directories(
  lib
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/lib>
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/lib/io>)

target_link_libraries(
  lib
  PUBLIC spdlog::spdlog
  PUBLIC ${HiSiSDK_LIBS}
  PUBLIC ${OpenCV_LIBS}
  PUBLIC QuFaceSDK::face
  PUBLIC QuFaceSDK::database
  PUBLIC Qt5::Widgets)

target_compile_definitions(lib PUBLIC -D${LCD_SCREEN_TYPE})
