cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${PROJECT_SOURCE_DIR}/cmake/himix200.toolchain.cmake"
      CACHE STRING "")
endif()

set(CMAKE_GENERATOR "Ninja")
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# set(CMAKE_DISABLE_SOURCE_CHANGES ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Set up ccache ...")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

project(
  FaceRecognitionTerminal
  VERSION 1.0.0
  DESCRIPTION "FaceRecognitionTerminal")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(DOWNLOAD_DEPENDENCY "Download 3rd party dependencies from remote" ON)
option(GENERATE_QT_TS_FILES "Generate qt ts files" OFF)

set(PROJECT_DEPENDENCY_DIR
    "${PROJECT_SOURCE_DIR}/deps"
    CACHE STRING "Project dependencies dir")
set(RESOURCE_PREFIX
    "${PROJECT_SOURCE_DIR}/resources"
    CACHE STRING "Dir for model & font file")
set(THIRD_PARTY_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/3rd"
    CACHE STRING "Dir for third party")
set(HISI_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/${HISI_SDK_PLATFORM}"
    CACHE STRING "Dir for hisi sdk")
set(QUFACE_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/qufacesdk"
    CACHE STRING "Dir for quface sdk")
set(QUFACE_IO_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/qufaceiosdk"
    CACHE STRING "Dir for quface io sdk")
set(QT_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/qtsdk"
    CACHE STRING "Dir for qt sdk")
set(TEMPERATURE_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/temperature"
    CACHE STRING "Dir for temperature sdk")

message(STATUS "set HISI_SDK_PREFIX to: ${HISI_SDK_PREFIX}")
message(STATUS "set THIRD_PARTY_PREFIX to: ${THIRD_PARTY_PREFIX}")
message(STATUS "set QUFACE_SDK_PREFIX to: ${QUFACE_SDK_PREFIX}")
message(STATUS "set QT_SDK_PREFIX to: ${QT_SDK_PREFIX}")
message(STATUS "set TEMPERATURE_SDK_PREFIX to: ${TEMPERATURE_SDK_PREFIX}")

set(PREFIX_LIST
    ${HISI_SDK_PREFIX} ${QUFACE_SDK_PREFIX} ${QUFACE_IO_SDK_PREFIX}
    ${THIRD_PARTY_PREFIX} ${QT_SDK_PREFIX} ${TEMPERATURE_SDK_PREFIX})

set(CMAKE_FIND_ROOT_PATH ${PREFIX_LIST})
set(CMAKE_PREFIX_PATH ${PREFIX_LIST})

if(DOWNLOAD_DEPENDENCY)
  # -- Download dependecies --
  include(cmake/download.cmake)

  set(DOWNLOAD_DIR "${PROJECT_DEPENDENCY_DIR}/download")

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/qt5-himix200-sdk.tgz
    FILENAME
    ${DOWNLOAD_DIR}/qt5-himix200-sdk.tgz
    HASH_TYPE
    SHA256
    HASH
    cef0d9f7fb8b300850813a6072c94ba7a9638b6d5b94ec3b52de9672d92d8ce6
    EXTRACT_DIR
    ${QT_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/deps/prebuild-3rd-0.2.0.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/prebuild-3rd-0.2.0.tar.gz
    HASH_TYPE
    SHA256
    HASH
    f3efdbbfff20837a141228efb4d91fe149186b4b0eeca84ccd8662d2574306ef
    EXTRACT_DIR
    ${THIRD_PARTY_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/v1-releases/QuFaceSDK-latest-hisi-rp-dv300.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/QuFaceSDK-latest-hisi-rp-dv300.tar.gz
    HASH_TYPE
    SHA256
    EXTRACT_DIR
    ${QUFACE_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/io-releases/QufaceIOSDK-latest-hisi-rp-dv300.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/QufaceIOSDK-latest-hisi-rp-dv300.tar.gz
    HASH_TYPE
    SHA256
    EXTRACT_DIR
    ${QUFACE_IO_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/rp-hi3516dv300-sdk-v2.tgz
    FILENAME
    ${DOWNLOAD_DIR}/rp-hi3516dv300-sdk-v2.tgz
    HASH_TYPE
    SHA256
    HASH
    0f50828123c0c0b682354e667d25e4aef4f05df2f3b956cc7ab3c597e4ba1bbb
    EXTRACT_DIR
    ${HISI_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/temperature-himix200.tgz
    FILENAME
    ${DOWNLOAD_DIR}/temperature-himix200.tgz
    HASH_TYPE
    SHA256
    HASH
    f30e0b80138bd20a65f00e0a0d178cae0fd20a633dcf56d31deb32619934cf44
    EXTRACT_DIR
    ${TEMPERATURE_SDK_PREFIX})
else()
  foreach(prefix ${PREFIX_LIST})
    if(NOT EXISTS ${prefix})
      message(STATUS "HINTS:     set DOWNLOAD_DEPENDENCY to ON")
      message(FATAL_ERROR "${prefix}: not exists!")
    endif()
  endforeach()
endif()

find_package(QuFaceSDK REQUIRED)
message(STATUS "Found QuFaceSDK ${QuFaceSDK_VERSION}")

find_package(QuFaceIOSDK REQUIRED)
message(STATUS "Found QuFaceIOSDK ${QuFaceIOSDK_VERSION}")

find_package(Qt5 REQUIRED COMPONENTS Widgets Charts LinguistTools)
message(STATUS "Found Qt5 ${Qt5_VERSION}")

find_package(httplib)

if(NOT QuFaceSDK_LOADED_DEPS)
  find_package(HiSiSDK REQUIRED HINTS ${QuFaceSDK_HISI_SDK_FIND_HINTS})
  find_package(OpenCV 3 REQUIRED HINTS /usr/local/opt/opencv@3)
  find_package(spdlog REQUIRED)
endif()

add_library(temperature::temperature STATIC IMPORTED)
set_target_properties(
  temperature::temperature
  PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${TEMPERATURE_SDK_PREFIX}/include"
             IMPORTED_LOCATION "${TEMPERATURE_SDK_PREFIX}/lib/libtemperature.a"
             INTERFACE_POSITION_INDEPENDENT_CODE "ON")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
endif()

configure_file("${PROJECT_SOURCE_DIR}/src/static_config.hpp.in"
               "${PROJECT_BINARY_DIR}/include/static_config.hpp")
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(src/app)
add_subdirectory(src/lib)
add_subdirectory(src/ui)
add_subdirectory(src/service)

set(TRANSLATION_TS_FILES i18n/face_terminal_en.ts i18n/face_terminal_zh-CN.ts
                         i18n/face_terminal_jp.ts)
if(GENERATE_QT_TS_FILES)
  set(TRANSLATION_SRC_FILES
      src/ui/recognize_tip_widget.cpp src/ui/screen_saver_widget.cpp
      src/app/record_task.cpp)
  qt5_create_translation(QM_FILES ${TRANSLATION_SRC_FILES}
                         ${TRANSLATION_TS_FILES})
else()
  qt5_add_translation(QM_FILES ${TRANSLATION_TS_FILES})
endif()

configure_file(i18n/translations.qrc ${PROJECT_BINARY_DIR} COPYONLY)

add_executable(face-terminal main.cpp resource.qrc
                             ${PROJECT_BINARY_DIR}/translations.qrc)
target_link_libraries(face-terminal PRIVATE ui)
install(TARGETS face-terminal DESTINATION .)

add_executable(temperature_task temperature_task.cpp resource.qrc)
target_link_libraries(temperature_task PRIVATE ui)
install(TARGETS temperature_task DESTINATION .)

add_executable(smoke-test smoke-test.cpp resource.qrc)
target_link_libraries(
  smoke-test
  PRIVATE
  PUBLIC lib)
install(TARGETS smoke-test DESTINATION .)
