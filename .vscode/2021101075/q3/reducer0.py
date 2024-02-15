#!/usr/bin/env python3
# -*-coding:utf-8 -*
# basically what it does it output the node and its distance and its neighbors
# since  1 is the source node so its distance is 0 and all other nodes are at a distance of 200000, neighbours is the list what they are connected to
# 2 200000 3:1 4:1 5:1  // it means that 2 is at distance 200000 and is connected to 3,4,5 who are at distance 1
import sys

SOURCE_NODE = 1

for line in sys.stdin:
    line = line.strip()
    node_key, node_neighbours = line.split("\t")

    node_dist = 0 if int(node_key) == SOURCE_NODE else 200000

    # Create node_adjacency_list
    node_adjacency_list = ["{}:{}".format(
        neighbour, 1) for neighbour in node_neighbours.split(",")]

    print('{}\t{}\t{}'.format(node_key, node_dist, ' '.join(node_adjacency_list)))
