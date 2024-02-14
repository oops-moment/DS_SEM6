#!/usr/bin/env python3
# -*-coding:utf-8 -*
import sys

current_node = None
friends_list = []

for line in sys.stdin:
    line = line.strip()
    node, friend = map(int, line.split('\t'))

    # If the node changes, output the current node and its friends
    if current_node is None:
        current_node = node

    if node != current_node:
        print(f"{current_node}\t{' '.join(map(str, friends_list))}")
        current_node = node
        friends_list = []

    friends_list.append(friend)

# Output the last node and its friends
if current_node is not None:
    print(f"{current_node}\t{' '.join(map(str, friends_list))}")
