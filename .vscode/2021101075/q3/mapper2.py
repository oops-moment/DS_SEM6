#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

for line in sys.stdin:
    # Split the input line into fields
    fields = line.strip().split('\t')

    # Extract relevant information
    current_node, current_dist, current_neighbours = fields[0], fields[1], fields[2:]
    current_dist = int(current_dist)
    if (current_dist <= 100):
        print(f"{current_node}\t{current_dist}")
