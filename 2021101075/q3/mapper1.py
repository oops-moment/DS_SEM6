#!/usr/bin/env python3
# -*-coding:utf-8 -*
# this mapper basically what it does is send the node along with its updated distance , so that the reducer can update the distance for that node
# 2 200000 3:1 4:1 5:1
# 2 1 neighbour path
# this goes to reducer and then 1 is decided as the distance and the path is updated

import sys

for line in sys.stdin:
    line = line.strip().split('\t')

    node = line[0]
    distance = line[1]

    # Use an empty string for neighbors if it is not present
    neighbours = line[2] if len(line) > 2 else ""
    distance = int(distance)

    print(f"{node}\t{distance}\t{neighbours}")

    if neighbours:
        for neighbour in neighbours.split(" "):
            adjacent_node, adjacent_distance = neighbour.strip().split(':', 1)
            adjacent_distance = int(adjacent_distance)
            adjacent_distance = distance + adjacent_distance
            print(f"{adjacent_node}\t{adjacent_distance}")
