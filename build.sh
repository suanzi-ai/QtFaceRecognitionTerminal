#!/bin/bash

set -e

export LC_ALL=C

source_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
build_root_dir="$source_dir/build"
install_dir=$source_dir/install

ENV_FILE="$source_dir/user.env"

if [ ! -f "$ENV_FILE" ]; then
    echo -e "-- Please copy $ENV_FILE.sample to $ENV_FILE and modify it"
    exit 1
fi

. $ENV_FILE

build_type=${USER_BUILD_TYPE:-"Release"}
cmake_generator=${USER_CMAKE_GENERATOR:-"Unix Makefiles"}

if [[ $USER_BUILD_DIR ]]; then
    build_root_dir=$USER_BUILD_DIR
fi

if [[ $USER_INSTALL_DIR ]]; then
    install_dir=$USER_INSTALL_DIR
fi

for i in "$@"; do
    case $i in
    -d=* | --build-dir=*)
        build_root_dir="${i#*=}"
        shift # past argument=value
        ;;
    --install-dir=*)
        install_dir="${i#*=}"
        shift # past argument=value
        ;;
    -t=* | --build-type=*)
        build_type="${i#*=}"
        shift # past argument=value
        ;;
    --ninja)
        cmake_generator="Ninja"
        shift # past argument with no value
        ;;
    *)
        # unknown option
        ;;
    esac
done

build_dir=$build_root_dir/$build_type

dep_dir=$source_dir/deps

QT_SDK_PREFIX="$dep_dir/qtsdk"
if [[ $USER_QT_SDK_PREFIX ]]; then
    QT_SDK_PREFIX=$USER_QT_SDK_PREFIX
fi

if [[ $USER_QT_SDK_HOME ]]; then
    QT_SDK_PREFIX=$USER_QT_SDK_HOME
fi

HISI_SDK_PREFIX="$dep_dir/rp-dv300"
if [[ $USER_HISI_SDK_PREFIX ]]; then
    HISI_SDK_PREFIX=$USER_HISI_SDK_PREFIX
fi

THIRD_PARTY_PREFIX="$dep_dir/3rd"
if [[ $USER_THIRD_PARTY_PREFIX ]]; then
    THIRD_PARTY_PREFIX=$USER_THIRD_PARTY_PREFIX
fi

TEMPERATURE_SDK_PREFIX="$dep_dir/temperature"
if [[ $USER_TEMPERATURE_SDK_PREFIX ]]; then
    TEMPERATURE_SDK_PREFIX=$USER_TEMPERATURE_SDK_PREFIX
fi

QUFACE_SDK_PREFIX="$dep_dir/qufacesdk"
if [[ $USER_QUFACE_SDK_PREFIX ]]; then
    QUFACE_SDK_PREFIX=$USER_QUFACE_SDK_PREFIX
fi

QUFACE_IO_SDK_PREFIX="$dep_dir/qufaceiosdk"
if [[ $USER_QUFACE_IO_SDK_PREFIX ]]; then
    QUFACE_IO_SDK_PREFIX=$USER_QUFACE_IO_SDK_PREFIX
fi

mkdir -p $build_dir || true
pushd $build_dir
cmake $source_dir -G "$cmake_generator" \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$install_dir \
    -DHISI_SDK_PLATFORM=rp-dv300 \
    -DPROJECT_DEPENDENCY_DIR=$dep_dir \
    -DQT_SDK_PREFIX=$QT_SDK_PREFIX \
    -DTHIRD_PARTY_PREFIX=$THIRD_PARTY_PREFIX \
    -DHISI_SDK_PREFIX=$HISI_SDK_PREFIX \
    -DQUFACE_SDK_PREFIX=$QUFACE_SDK_PREFIX \
    -DQUFACE_IO_SDK_PREFIX=$QUFACE_IO_SDK_PREFIX \
    -DTEMPERATURE_SDK_PREFIX=$TEMPERATURE_SDK_PREFIX \
    -DCMAKE_TOOLCHAIN_FILE=$source_dir/cmake/himix200.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=$build_type

cmake --build . -- -j 4
cmake --build . --target install
popd

if [[ $USER_SCP_COPY_TO ]]; then
    echo "Copy to $USER_SCP_COPY_TO"
    scp $build_dir/face-terminal $USER_SCP_COPY_TO
else
    echo -e "\n#####################################################\n"
    echo -e "请将下面的文件夹内容拷贝至开发板后，手动运行程序即可: "
    echo -e "\t $install_dir"
    ls -lh $install_dir
    echo -e "\n#####################################################\n"
fi
