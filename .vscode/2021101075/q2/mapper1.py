#!/usr/bin/env python3
# -*-coding:utf-8 -*
# Basically what this does is that it uses the output from reducer 0 1 : 2 3 4 5, then it does for all pairs:
#  2_3\t1  2_4\t1 2_5\t1 3_4\t1 3_5\t1 4_5\t1

import sys

for line in sys.stdin:
    line = line.strip()
    node, friends_str = line.split('\t')
    node = int(node)
    friends = list(map(int, friends_str.split()))

    # Emit pair need to be considered
    for i in range(len(friends)):
        for j in range(i + 1, len(friends)):
            if(friends[i] > friends[j]):
                print(f"{friends[j]}_{friends[i]}\t{node}")
            else:
                print(f"{friends[i]}_{friends[j]}\t{node}")
