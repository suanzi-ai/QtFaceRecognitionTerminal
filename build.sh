#!/bin/bash

set -e

export LC_ALL=C

ENV_FILE="user.env"

if [ ! -f "$ENV_FILE" ]; then
    echo -e "-- Please copy $ENV_FILE.sample to $ENV_FILE and modify it"
    exit 1
fi

. $ENV_FILE

if [[ $USER_QT_SDK_HOME ]]; then
    echo -e "-- Found USER_QT_SDK_HOME=$USER_QT_SDK_HOME"
else
    echo -e "-- Please specify the QT SDK Home:"
    echo -e "--    'export USER_QT_SDK_HOME=path/to/qtsdk'"
    exit 1
fi

build_type=${USER_BUILD_TYPE:-"Release"}
cmake_generator=${USER_CMAKE_GENERATOR:-"Unix Makefiles"}

source_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
build_root_dir="$source_dir/build"
clean_3rd="no"

if [[ $USER_BUILD_DIR ]]; then
    build_root_dir=$USER_BUILD_DIR
fi

for i in "$@"; do
    case $i in
    -d=* | --build-dir=*)
        build_root_dir="${i#*=}"
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
    --clean-3rd)
        clean_3rd="yes"
        shift # past argument with no value
        ;;
    *)
        # unknown option
        ;;
    esac
done

build_dir=$build_root_dir/$build_type
dep_dir=$source_dir/deps
install_dir=$build_dir/install

dep_sub_dirs="qufacesdk 3rd rp-dv300 resources"
if [[ $clean_3rd == "yes" ]]; then
    echo "Cleaning dep dir ..."
    for d in $dep_sub_dirs; do
        rm -rf $source_dir/deps/$d || true
        echo "Clean dir: $source_dir/deps/$d"
    done
    echo "Cleaning dep dir done"
else
    for d in $dep_sub_dirs; do
        if [ -d $source_dir/deps/$d ]; then
            echo "[WARN] '$source_dir/deps/$d' exists, won't update"
        fi
    done
fi

mkdir -p $build_dir || true
pushd $build_dir
cmake $source_dir -G "$cmake_generator" \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$install_dir \
    -DDOWNLOAD_DEPENDENCY=OFF \
    -DPROJECT_DEPENDENCY_DIR=$dep_dir \
    -DQT_SDK_PREFIX=$USER_QT_SDK_HOME \
    -DTHIRD_PARTY_PREFIX=$USER_THIRD_PARTY_PREFIX \
    -DQUFACE_SDK_PREFIX=$USER_QUFACE_SDK_PREFIX \
    -DCMAKE_TOOLCHAIN_FILE=$source_dir/cmake/himix200.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=$build_type

cmake --build . -- -j 4
cmake --build . --target install
popd

if [[ $USER_SCP_COPY_TO ]]; then
    echo "Copy to $USER_SCP_COPY_TO"
    scp $build_dir/main $USER_SCP_COPY_TO
else
    echo -e "\n#####################################################\n"
    echo -e "请将下面的文件夹内容拷贝至开发板后，手动运行程序即可: "
    echo -e "\t $install_dir"
    ls -lh $install_dir
    echo -e "\n#####################################################\n"
fi
