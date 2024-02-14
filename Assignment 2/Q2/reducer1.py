#!/usr/bin/env python3
# -*-coding:utf-8 -*

import sys

current_key = None
common_friends = None

def format_output(key):
    return tuple(map(int, key.split('_')))

for line in sys.stdin:
    line = line.strip()
    key, friends_str = line.split('\t')
    friends = set(map(int, friends_str.split()))

    if current_key is None:
        current_key = key
        common_friends = friends
    elif key == current_key:
        common_friends = common_friends.intersection(friends)
    else:
        if(len(common_friends)>0):
            formatted_key = format_output(current_key)
            print(f"{formatted_key[0]} {formatted_key[1]}\t{' '.join(map(str, common_friends))}")
        current_key = key
        common_friends = friends

# Output the final result for the last key
if current_key is not None:
    if(len(common_friends)>0):
            formatted_key = format_output(current_key)
            print(f"{formatted_key[0]} {formatted_key[1]}\t{' '.join(map(str, common_friends))}")
