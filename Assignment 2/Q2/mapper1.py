#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

for line in sys.stdin:
    line = line.strip()

    node, friends_str = line.split('\t')
    node = int(node)
    friends = list(map(int, friends_str.split()))

    # Emit pairs of node_friend and friends list
    for friend in friends:
        if(node > friend):
            print(f"{friend}_{node}\t{' '.join(map(str, friends))}")
        else:
            print(f"{node}_{friend}\t{' '.join(map(str, friends))}")
