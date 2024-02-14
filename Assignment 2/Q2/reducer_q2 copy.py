#!/usr/bin/env python3
# -*-coding:utf-8 -*   
import sys
from itertools import combinations

result_dict = {}

for line in sys.stdin:
    line = line.strip()
    key, value = line.split('\t', 1)

    if key in result_dict:
        result_dict[key].append(value)
    else:
        result_dict[key] = [value]

# Generate combinations of keys and find mutual friends
for pair_keys in combinations(result_dict.keys(), 2):
    key1, key2 = pair_keys
    friends_list1 = result_dict[key1]
    friends_list2 = result_dict[key2]

    mutual_friends = set.intersection(*map(set, [friends_list1, friends_list2]))
    if mutual_friends:
        print(f"{key1} {key2}\t{','.join(map(str, sorted(mutual_friends)))}")
