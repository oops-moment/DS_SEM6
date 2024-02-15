#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

for line in sys.stdin:
    line = line.strip()
    node, friends_str = line.split('\t')
    node = int(node)
    friends = list(map(int, friends_str.split()))

    # Emit pairs of node_friend and 1
    for i in range(len(friends)):
        for j in range(i + 1, len(friends)):
            if(friends[i] > friends[j]):
                print(f"{friends[j]}_{friends[i]}\t{node}")
            else:
                print(f"{friends[i]}_{friends[j]}\t{node}")
