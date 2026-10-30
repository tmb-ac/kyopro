#!/usr/bin/env bash

set -u

if (( $# == 0 )); then
    echo "usage: $0 <source.cpp | source> [program arguments ...]" >&2
    exit 2
fi

ROOT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
SOURCE=$1
shift
[[ $SOURCE == *.cpp ]] || SOURCE+=.cpp
[[ $SOURCE = /* ]] || SOURCE="$ROOT_DIR/$SOURCE"

CXX=${CXX:-g++-15}
if ! command -v "$CXX" >/dev/null 2>&1; then
    echo "$CXX was not found." >&2
    echo "Install GCC 15, or temporarily run: CXX=g++ $0 ${SOURCE%.cpp}" >&2
    exit 127
fi
if [[ ! -f $SOURCE ]]; then
    echo "source file not found: $SOURCE" >&2
    exit 2
fi

ACL_DIR=${ACL_DIR:-/home/eevee/ac-library-master}
BUILD_DIR="$ROOT_DIR/file/.build"
PCH_NAME=stdcxx
PCH_INCLUDES=('#include <bits/stdc++.h>')
if grep -Eq '^[[:space:]]*#[[:space:]]*include[[:space:]]*[<"]boost/multiprecision/cpp_int\.hpp[>"]' "$SOURCE"; then
    PCH_NAME=stdcxx_boost_cpp_int
    PCH_INCLUDES+=('#include <boost/multiprecision/cpp_int.hpp>')
fi
PCH="$BUILD_DIR/$PCH_NAME.hpp"
PCH_GCH="$PCH.gch"
PCH_CONFIG="$BUILD_DIR/$PCH_NAME.config"
OUTPUT="$ROOT_DIR/file/out.exe"
TEMP_OUTPUT="$BUILD_DIR/out.exe.tmp"

mkdir -p "$BUILD_DIR"

COMPILE_FLAGS=(-std=gnu++23 -O2 -pipe -I "$ROOT_DIR")
[[ -d $ACL_DIR ]] && COMPILE_FLAGS+=(-I "$ACL_DIR")

# bits/stdc++.h の解析結果をコンパイラごとにキャッシュする。
CXX_PATH=$(command -v "$CXX")
CXX_VERSION=$($CXX -dumpfullversion -dumpversion)
CONFIG="$CXX_PATH $CXX_VERSION ${COMPILE_FLAGS[*]}"
if [[ ! -f $PCH_GCH || ! -f $PCH_CONFIG || $(<"$PCH_CONFIG") != "$CONFIG" ]]; then
    printf '%s\n' "${PCH_INCLUDES[@]}" > "$PCH"
    echo "building precompiled header ($CXX $CXX_VERSION; first run only)..."
    if ! "$CXX" "${COMPILE_FLAGS[@]}" -x c++-header "$PCH" -o "$PCH_GCH"; then
        rm -f "$PCH_GCH" "$PCH_CONFIG"
        exit 1
    fi
    printf '%s\n' "$CONFIG" > "$PCH_CONFIG"
fi

if "$CXX" "${COMPILE_FLAGS[@]}" -include "$PCH" "$SOURCE" -o "$TEMP_OUTPUT"; then
    mv -f "$TEMP_OUTPUT" "$OUTPUT"
    echo "compiled ${SOURCE#"$ROOT_DIR/"} ($CXX $CXX_VERSION)"
    echo "executing..."
    "$OUTPUT" "$@"
else
    rm -f "$TEMP_OUTPUT"
    exit 1
fi
