#!/usr/bin/env bash

set -u

root_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
random_dir="$root_dir/random_test"
failure_dir="$random_dir/failure"

if (( $# < 1 || $# > 2 )); then
    echo "使い方: $0 <提出ファイルの番号> [テスト回数]" >&2
    echo "例:     $0 1 10000" >&2
    echo "回数を省略すると、間違いが見つかるまで実行します。" >&2
    exit 2
fi

target=$1
limit=${2:-0}

if [[ ! $target =~ ^[0-9]+$ ]] || [[ ! -f "$root_dir/$target.cpp" ]]; then
    echo "エラー: $target.cpp が見つかりません。" >&2
    exit 2
fi
if [[ ! $limit =~ ^[0-9]+$ ]]; then
    echo "エラー: テスト回数には 0 以上の整数を指定してください。" >&2
    exit 2
fi
if [[ ! -f "$random_dir/test.cpp" || ! -f "$random_dir/greed.cpp" ]]; then
    echo "エラー: random_test/test.cpp と random_test/greed.cpp が必要です。" >&2
    exit 2
fi

cxx=${CXX:-g++}
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/random-test.XXXXXX")
trap 'rm -rf "$build_dir"' EXIT INT TERM

common_flags=(-std=c++23 -O2 -Wall -Wextra -I "$root_dir")
if [[ -d /home/eevee/ac-library-master ]]; then
    common_flags+=(-I /home/eevee/ac-library-master)
fi

compile() {
    local source=$1
    local output=$2
    echo "compile: $source"
    "$cxx" "${common_flags[@]}" "$source" -o "$output"
}

compile "$root_dir/$target.cpp" "$build_dir/main" || exit 1
compile "$random_dir/greed.cpp" "$build_dir/greed" || exit 1
compile "$random_dir/test.cpp" "$build_dir/generator" || exit 1

mkdir -p "$failure_dir"

seed_base=${SEED:-$(date +%s)}
if [[ ! $seed_base =~ ^[0-9]+$ ]]; then
    echo "エラー: SEED には 0 以上の整数を指定してください。" >&2
    exit 2
fi
case_no=1
while (( limit == 0 || case_no <= limit )); do
    seed=$((seed_base + case_no - 1))

    if ! "$build_dir/generator" "$seed" > "$build_dir/input.txt"; then
        echo "generator failed (seed=$seed)" >&2
        exit 1
    fi

    timeout "${TIMEOUT:-2s}" "$build_dir/main" \
        < "$build_dir/input.txt" > "$build_dir/main.txt"
    status=$?
    if (( status != 0 )); then
        cp "$build_dir/input.txt" "$failure_dir/input.txt"
        echo "$seed" > "$failure_dir/seed.txt"
        echo "提出コードが異常終了または時間超過しました (case=$case_no, seed=$seed, status=$status)"
        echo "失敗ケース: random_test/failure/input.txt"
        exit 1
    fi

    timeout "${TIMEOUT:-2s}" "$build_dir/greed" \
        < "$build_dir/input.txt" > "$build_dir/greed.txt"
    status=$?
    if (( status != 0 )); then
        cp "$build_dir/input.txt" "$failure_dir/input.txt"
        echo "$seed" > "$failure_dir/seed.txt"
        echo "愚直解が異常終了または時間超過しました (case=$case_no, seed=$seed, status=$status)" >&2
        exit 1
    fi

    if ! diff -Z -B "$build_dir/greed.txt" "$build_dir/main.txt" > /dev/null; then
        cp "$build_dir/input.txt" "$failure_dir/input.txt"
        cp "$build_dir/main.txt" "$failure_dir/main.txt"
        cp "$build_dir/greed.txt" "$failure_dir/greed.txt"
        echo "$seed" > "$failure_dir/seed.txt"

        echo
        echo "Wrong Answer (case=$case_no, seed=$seed)"
        echo "input:"
        cat "$failure_dir/input.txt"
        echo "expected (greed.cpp):"
        cat "$failure_dir/greed.txt"
        echo "actual ($target.cpp):"
        cat "$failure_dir/main.txt"
        echo "失敗時のファイルは random_test/failure/ に保存しました。"
        exit 1
    fi

    if (( case_no % 100 == 0 )); then
        echo "accepted: $case_no cases (last seed=$seed)"
    fi
    ((case_no++))
done

echo "All $limit cases passed."
