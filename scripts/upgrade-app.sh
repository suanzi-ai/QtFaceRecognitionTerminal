#!/bin/bash

set -e

remote=${1:-"192.168.2.19"}
user=${2:-"123456"}
pass=${3:-"123456"}
upgrade_pkg=build/face-terminal.tgz

body=$(curl -s -d "{\"user\": \"$user\", \"password\": \"$pass\"}" -H "Content-Type: application/json" http://$remote/adm-api/v1/login)

token=$(printf "%s" $body | jq '.token')
token="${token%\"}"
token="${token#\"}"

curl -F "file=@$upgrade_pkg" -H "Authorization: Bearer $token" http://$remote/adm-api/v1/systems/upgradeApp
