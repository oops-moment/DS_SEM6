#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

for line in sys.stdin:
    line = line.strip().split('\t')

    node = line[0]
    distance = line[1]
    
    # Use an empty string for neighbors if it is not present
    neighbours = line[2] if len(line) > 2 else ""

    if len(line) == 4:
        path = line[3]
    else:
        path = node

    distance = int(distance)

    print(f"{node}\t{distance}\t{neighbours}\t{path}")

    if neighbours:
        for neighbour in neighbours.split(" "):
            adjacent_node, adjacent_distance = neighbour.strip().split(':', 1)
            adjacent_distance = int(adjacent_distance)
            adjacent_distance = distance + adjacent_distance
            child_path = '{}>{}'.format(path, str(adjacent_node))
            print(f"{adjacent_node}\t{adjacent_distance}\t{child_path}")
