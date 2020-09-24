#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd $DIR/test
cmake CMakeLists.txt
make
build_test/Rag3Test

if [[ $? -eq 0 ]] ; then 
    src/engine/j3x/semantic_checker_test.sh
else
    exit 1
fi