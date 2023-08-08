#!/bin/sh
files_to_format=$(git ls-files | grep -E '.*\.(cpp|h|hpp)$' | grep -v -E '3rdparty')
echo "$files_to_format" | xargs clang-format --style=file -i