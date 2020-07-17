#!/bin/bash

set -e

tmp_dir=/tmp/quface-upgrade
face_terminal=~/build-cache/qtdemo/Release/main

rm -rf $tmp_dir || true

mkdir -p $tmp_dir/bin

cp scripts/start-upgrade.sh $tmp_dir/

cp $face_terminal $tmp_dir/bin/face-terminal

tar -C $tmp_dir -cvzf build/face-terminal.tgz .
