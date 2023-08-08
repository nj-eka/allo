#!/bin/bash
CLANG_TIDY=${1:-"clang-tidy"}
files_to_format=$(git ls-files | grep -E '.*\.(cpp|h|hpp)$' | grep -v -E '3rdparty')
$CLANG_TIDY --format-style=file -p "build/compile_commands.json" -checks="-*,portability-*,bugprone-*,readability-;" $files_to_format
