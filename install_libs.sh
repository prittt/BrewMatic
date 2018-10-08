#!/bin/bash

project_dir="BrewMaticAlpha01"

echo "Please, insert Arduino libraries path, usually /Users/<username>/Documents/Arduino/libraries on mac"
read arduino_path

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"

#echo cur path: $cur_path

if [ ! -d "$arduino_path" ]; then
    echo "The specified directory doesn't exist, quit."
    exit 1
fi      

#Move into Arduino's libraries folder
cd $arduino_path

#new_path=$(pwd)
#echo new path: $new_path

# Iterate on variables
for f in $SCRIPTPATH/$project_dir/libs/*; do
    if [[ -d $f ]]; then
        # $f is a directory
        #echo $f $project_dir$(basename $f)
        ln -s $f $project_dir$(basename $f)
    fi
done