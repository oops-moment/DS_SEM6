#!/usr/bin/env python3
# -*-coding:utf-8 -*
# 1 236  : 1 2,3,6

import sys
for line in sys.stdin:
    line = line.strip()
    node, *neighbors = line.split()
    print(f"{node}\t{','.join(neighbors)}")
