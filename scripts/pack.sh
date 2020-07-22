#!/bin/bash

set -e

tmp_dir=/tmp/quface-upgrade
face_terminal=~/build-cache/qtdemo/Release/main
face_model=~/build-cache/qtdemo/facemodel.bin

rm -rf $tmp_dir || true

mkdir -p $tmp_dir/bin

cp scripts/start-upgrade.sh $tmp_dir/

cp $face_terminal $tmp_dir/bin/face-terminal

# if [[ -f $face_model ]]; then
#     mkdir -p $tmp_dir/var/face-terminal
#     cp $face_model $tmp_dir/var/face-terminal/
# fi

tar -C $tmp_dir -cvzf build/face-terminal.tgz .
