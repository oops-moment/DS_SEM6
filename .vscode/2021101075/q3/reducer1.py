#!/usr/bin/env python3
# -*-coding:utf-8 -*
# basically this reducer get multiple inputs from the mapper and then it updates the distance and the path for each node
# 2 200000 3:1 4:1 5:1 and 2 1 neighbour path then it takes 1 since that is minimum

import sys

current_dist = None
current_node = None
current_neighbours = 0

for line in sys.stdin:
    line = line.strip().split('\t')
    node = line[0]
    distance = line[1]
    if len(line) == 2:
        # child updation
        neighbours = 0

    if len(line) == 3:
        # node information
        neighbours = line[2] if line[2] != "0" else 0

    distance = int(distance)
    # now each node we need the new distance and that has to be the smallest possible among all the outputs for that node

    if current_node == node:
        if distance < current_dist:
            current_dist = distance

        if neighbours != 0:
            current_neighbours = neighbours
    else:

        if current_node:
            print(
                f"{current_node}\t{current_dist}\t{current_neighbours}")

        current_node = node
        current_dist = distance
        current_neighbours = neighbours

if current_node == node:
    print(f"{current_node}\t{current_dist}\t{current_neighbours}")
