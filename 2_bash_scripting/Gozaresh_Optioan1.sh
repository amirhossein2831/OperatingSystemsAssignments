#!/bin/bash

read -p "enter the path: " path

cd "$(eval echo $path)" || { echo "Path not found."; exit 1; }

echo $(ls)
