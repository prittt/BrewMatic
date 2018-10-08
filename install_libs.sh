#!/bin/bash
project_dir="BrewMaticAlpha02"

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

arduino_path="/Users/federicobolelli/Documents/Arduino/libraries"

if [ ! -d "$arduino_path" ]; then
    echo The default Arduino\'s libraries path \"$arduino_path\" doesn\'t exist, please insert a valid one:
    read arduino_path
    while [ ! -d "$arduino_path" ]; do
        echo The path doesn\'t exist, please insert a valid one:
        read arduino_path
    done
fi

#Move into Arduino's libraries folder
cd $arduino_path

#new_path=$(pwd)
#echo new path: $new_path

# Iterate on variables
for f in $SCRIPTPATH/$project_dir/libs/*; do
    if [[ -d $f ]]; then
        # $f is a directory
        new_name=$project_dir$(basename $f)
        if [[ ! -d $new_name ]]; then
            ln -s $f $new_name
        fi
    fi
done