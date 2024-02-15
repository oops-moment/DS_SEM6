#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

SOURCE_NODE = 1

for line in sys.stdin:
    line = line.strip()
    node_key, node_neighbours = line.split("\t")

    node_dist = 0 if int(node_key) == SOURCE_NODE else 200000  

    # Create node_adjacency_list
    node_adjacency_list = ["{}:{}".format(neighbour, 1) for neighbour in node_neighbours.split(",")]

    print('{}\t{}\t{}'.format(node_key, node_dist, ' '.join(node_adjacency_list)))
