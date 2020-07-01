cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${PROJECT_SOURCE_DIR}/cmake/himix200.toolchain.cmake"
      CACHE STRING "")
endif()

set(CMAKE_GENERATOR "Ninja")
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Set up ccache ...")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

project(
  FaceRecognitionTerminal
  VERSION 0.1.0
  DESCRIPTION "FaceRecognitionTerminal")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(DOWNLOAD_DEPENDENCY "Download 3rd party dependencies from remote" ON)

set(PROJECT_DEPENDENCY_DIR
    "${PROJECT_SOURCE_DIR}/deps"
    CACHE STRING "Project dependencies dir")
set(RESOURCE_PREFIX
    "${PROJECT_SOURCE_DIR}/resources"
    CACHE STRING "Dir for model & font file")
set(HISI_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/${HISI_SDK_PLATFORM}"
    CACHE STRING "Dir for hisi sdk")
set(THIRD_PARTY_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/3rd"
    CACHE STRING "Dir for 3rd")
set(QUFACE_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/qufacesdk"
    CACHE STRING "Dir for quface sdk")
set(QT_SDK_PREFIX
    "${PROJECT_DEPENDENCY_DIR}/qtsdk"
    CACHE STRING "Dir for qt sdk")

message(STATUS "set HISI_SDK_PREFIX to: ${HISI_SDK_PREFIX}")
message(STATUS "set THIRD_PARTY_PREFIX to: ${THIRD_PARTY_PREFIX}")
message(STATUS "set QUFACE_SDK_PREFIX to: ${QUFACE_SDK_PREFIX}")
message(STATUS "set QT_SDK_PREFIX to: ${QT_SDK_PREFIX}")

set(CMAKE_FIND_ROOT_PATH ${HISI_SDK_PREFIX} ${THIRD_PARTY_PREFIX}
                         ${QUFACE_SDK_PREFIX} ${QT_SDK_PREFIX})
set(CMAKE_PREFIX_PATH ${HISI_SDK_PREFIX} ${THIRD_PARTY_PREFIX}
                      ${QUFACE_SDK_PREFIX} ${QT_SDK_PREFIX})

if(DOWNLOAD_DEPENDENCY)
  # -- Download dependecies --
  include(cmake/download.cmake)

  set(DOWNLOAD_DIR "${PROJECT_DEPENDENCY_DIR}/download")

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/deps/prebuild-3rd-0.1.3.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/prebuild-3rd-0.1.3.tar.gz
    HASH_TYPE
    SHA256
    HASH
    ce34e1a94997f7980e67b4a681184a52bccbf7d4c8e19f347e8aed77f3fe6dfd
    EXTRACT_DIR
    ${THIRD_PARTY_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/hisi/rp-dv300-sdk.tgz
    FILENAME
    ${DOWNLOAD_DIR}/rp-dv300-sdk.tgz
    HASH_TYPE
    SHA256
    HASH
    f9075af18adbb95a21789f6d79ca98fa1f92fee0696a2d156a88425fa4efdbcb
    EXTRACT_DIR
    ${HISI_SDK_PREFIX})

  download_and_extract(
    URL
    https://quvision.oss-cn-beijing.aliyuncs.com/qufacesdk/models/hisi-model-latest.tar.gz
    FILENAME
    ${DOWNLOAD_DIR}/hisi-model-latest.tar.gz
    HASH_TYPE
    SHA256
    EXTRACT_DIR
    ${RESOURCE_PREFIX}/models
    REMOVE_EXTRACT_DIR_IF_EXISTS)
endif()

find_package(QuFaceSDK REQUIRED)
message(STATUS "Found QuFaceSDK ${QuFaceSDK_VERSION}")

find_package(Qt5 REQUIRED COMPONENTS Widgets)
message(STATUS "Found Qt5 ${Qt5_VERSION}")

add_subdirectory(src/app)
add_subdirectory(src/lib)
add_subdirectory(src/ui)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE ui)

install(TARGETS main DESTINATION .)
install(DIRECTORY ${RESOURCE_PREFIX}/models/ DESTINATION .)
