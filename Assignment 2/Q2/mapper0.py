#!/usr/bin/env python3
# -*-coding:utf-8 -*
import sys

for line in sys.stdin:
    line = line.strip()
    friends = list(map(int, line.split()))
    x, y = friends

    # Output (x, y) pair
    print(f"{x}\t{y}")

    # Output (y, x) pair

    print(f"{y}\t{x}")

    