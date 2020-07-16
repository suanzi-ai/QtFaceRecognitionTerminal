#!/bin/bash

set -e

home_dir=/user/quface-app

$home_dir/bin/supervisord -c $home_dir/etc/supervisord.conf ctl restart face-terminal
