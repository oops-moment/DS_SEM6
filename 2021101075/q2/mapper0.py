#!/usr/bin/env python3
# -*-coding:utf-8 -*

# This basically takes into account all pair of friends and output them as (x,y) and (y,x) pairs
#  1 2 , then it sends to the reducer 1 2 and 2 1 both respectively
import sys

for line in sys.stdin:
    line = line.strip()
    friends = list(map(int, line.split()))
    x, y = friends

    # Output (x, y) pair
    print(f"{x}\t{y}")

    # Output (y, x) pair

    print(f"{y}\t{x}")
