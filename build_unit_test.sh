#!/bin/bash

# use the CXX envvar to specify an alternative compiler (must be in PATH, otherwise specify with full path), like this:
#
# $ CXX=alt_compiler ./build_unit_test.sh

if [[ -z ${CXX} ]]; then
	CXX=clang++-3.7
fi

SRC=(
	main.cpp
	test_virtual/test_virtual.cpp
	test_virtual/handmade_virtual.cpp
	test_inline/test_inline_impl.cpp
	test_pointer_alias/test_pointer_alias.cpp
	test_pointer_alias/test_pointer_alias_impl.cpp
)
BUILD_CMD=${CXX}" "${SRC[@]}" -std=c++14 -I. -lpthread -O3 -DCXXOPTS_NO_RTTI -fno-rtti -fstrict-aliasing -ffast-math -march=native -mtune=native -DQUIRK_001"
echo $BUILD_CMD
$BUILD_CMD
