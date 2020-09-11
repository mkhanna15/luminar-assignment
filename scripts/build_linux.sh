#!/bin/bash

./scripts/generate_linux_makefiles.sh

pushd build

cmake --build . -- -j 4

popd
