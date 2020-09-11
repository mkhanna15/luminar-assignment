#!/bin/bash

rm -rf ./build
mkdir build

pushd build

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" ..

popd
