#!/usr/bin/env python3
"""Estimate how many fixed-performance contests are needed for an AtCoder rating goal."""

from __future__ import annotations

import argparse
import json
import math
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path
from typing import Any


HISTORY_URL = "https://atcoder.jp/users/{user}/history/json"
DECAY = 0.9


def participation_penalty(contests: int) -> float:
    """Return AtCoder's correction for having participated only a few times."""
    numerator = math.sqrt(sum(DECAY ** (2 * i) for i in range(1, contests + 1)))
    denominator = sum(DECAY**i for i in range(1, contests + 1))
    limit = math.sqrt(19.0) / 19.0
    return 1200.0 * (numerator / denominator - limit) / (1.0 - limit)


def rating_from_performances(performances: list[int]) -> float:
    """Calculate the unrounded Algorithm rating from oldest-to-newest performances."""
    if not performances:
        return 0.0

    weighted_sum = 0.0
    weight_sum = 0.0
    weight = 1.0
    for performance in reversed(performances):
        weighted_sum += weight * 2.0 ** (performance / 800.0)
        weight_sum += weight
        weight *= DECAY

    raw_rating = 800.0 * math.log2(weighted_sum / weight_sum)
    raw_rating -= participation_penalty(len(performances))

    # AtCoder applies this transform only to ratings below 400.
    if raw_rating < 400.0:
        return 400.0 * math.exp((raw_rating - 400.0) / 400.0)
    return raw_rating


def displayed_rating(rating: float) -> int:
    """Round a non-negative rating in the same direction as AtCoder."""
    return math.floor(rating + 0.5)


def fetch_history(user: str, timeout: float) -> list[dict[str, Any]]:
    quoted_user = urllib.parse.quote(user, safe="")
    request = urllib.request.Request(
        HISTORY_URL.format(user=quoted_user),
        headers={"User-Agent": "atcoder-rating-goal/1.0"},
    )
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            data = json.load(response)
    except urllib.error.HTTPError as error:
        if error.code == 404:
            raise RuntimeError(f"AtCoder user not found: {user}") from error
        raise RuntimeError(f"AtCoder returned HTTP {error.code}") from error
    except (urllib.error.URLError, TimeoutError, json.JSONDecodeError) as error:
        raise RuntimeError(f"failed to fetch AtCoder history: {error}") from error

    if not isinstance(data, list):
        raise RuntimeError("AtCoder returned an unexpected history format")
    return data


def load_history(path: Path) -> list[dict[str, Any]]:
    try:
        with path.open(encoding="utf-8") as file:
            data = json.load(file)
    except (OSError, json.JSONDecodeError) as error:
        raise RuntimeError(f"failed to read history JSON: {error}") from error
    if not isinstance(data, list):
        raise RuntimeError("history JSON must contain an array")
    return data


def rated_performances(history: list[dict[str, Any]]) -> list[int]:
    result: list[int] = []
    for entry in history:
        if not entry.get("IsRated", False):
            continue
        performance = entry.get("InnerPerformance")
        if not isinstance(performance, int):
            raise RuntimeError("a rated history entry has no integer InnerPerformance")
        result.append(performance)
    return result


def contests_needed(
    performances: list[int], performance: int, target: int, max_contests: int
) -> tuple[int, list[int]]:
    projected: list[int] = []
    for count in range(max_contests + 1):
        rating = displayed_rating(rating_from_performances(performances))
        projected.append(rating)
        if rating >= target:
            return count, projected
        performances.append(performance)
    raise RuntimeError(
        f"the target was not reached within {max_contests} additional contests"
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Calculate how many fixed-performance contests are needed to reach an AtCoder Algorithm rating."
    )
    parser.add_argument("--user", default="tmb", help="AtCoder user name (default: tmb)")
    parser.add_argument(
        "--performance", type=int, default=2200, help="future performance X (default: 2200)"
    )
    parser.add_argument("--target", type=int, default=2000, help="target rating (default: 2000)")
    parser.add_argument("--timeout", type=float, default=15.0, help="HTTP timeout in seconds")
    parser.add_argument(
        "--history-json",
        type=Path,
        help="read a saved AtCoder history JSON instead of downloading it",
    )
    parser.add_argument("--max-contests", type=int, default=10000, help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.performance < 0 or args.target < 0:
        parser.error("--performance and --target must be non-negative")
    if args.timeout <= 0 or args.max_contests < 0:
        parser.error("--timeout must be positive")
    return args


def main() -> int:
    args = parse_args()
    try:
        history = (
            load_history(args.history_json)
            if args.history_json is not None
            else fetch_history(args.user, args.timeout)
        )
        performances = rated_performances(history)
        if not performances:
            raise RuntimeError(f"{args.user} has no rated Algorithm contest history")

        current = displayed_rating(rating_from_performances(performances))
        count, projected = contests_needed(
            performances.copy(), args.performance, args.target, args.max_contests
        )
    except RuntimeError as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(f"user: {args.user}")
    print(f"rated contests: {len(performances)}")
    print(f"current calculated rating: {current}")
    print(f"assumed performance: {args.performance}")
    print(f"target rating: {args.target}")
    for index, rating in enumerate(projected[1:], start=1):
        print(f"after {index} contest(s): {rating}")
    print(f"contests needed: {count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
