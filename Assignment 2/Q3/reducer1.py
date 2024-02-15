#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

current_dist = None
current_node = None
current_path = None
current_neighbours = 0

for line in sys.stdin:
    line = line.strip().split('\t')
    node = line[0]
    distance = line[1]
    if len(line) == 3:
        # child updation
        path = line[2]
        neighbours = 0

    if len(line) == 4:
        # node information
        path = line[3]
        neighbours = line[2] if line[2] != "0" else 0

    distance = int(distance)
    # now each node we need the new distance and that has to be the smallest possible among all the outputs for that node

    if current_node == node:
        if distance < current_dist:
            current_dist = distance
            current_path = path
        
        if neighbours != 0:
            current_neighbours = neighbours
    else:
      
        if current_node:
            print (f"{current_node}\t{current_dist}\t{current_neighbours}\t{current_path}")
        
        current_node = node
        current_dist = distance
        current_path = path
        current_neighbours = neighbours

if current_node == node:
    print (f"{current_node}\t{current_dist}\t{current_neighbours}\t{current_path}")