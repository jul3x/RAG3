#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd $DIR/test
cmake CMakeLists.txt
make
build_test/Rag3Test
