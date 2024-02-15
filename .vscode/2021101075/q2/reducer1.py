#!/usr/bin/env python3
# -*-coding:utf-8 -*
# now for the same keys you need to combine the output basically 1_2 :3, 1_2 :4 then 1_2 : 3 4
import sys

current_key = None
values = []


def format_output(key):
    return tuple(map(int, key.split('_')))


for line in sys.stdin:
    line = line.strip()
    key, value = line.split('\t')

    if current_key is None:
        current_key = key
        values.append(int(value))
    elif key == current_key:
        values.append(int(value))
    else:
        formatted_key = format_output(current_key)
        sorted_values = sorted(values)
        print(
            f"{formatted_key[0]} {formatted_key[1]}\t{' '.join(map(str, sorted_values))}")
        current_key = key
        values = [int(value)]

# Output the final result for the last key
if current_key is not None:
    formatted_key = format_output(current_key)
    sorted_values = sorted(values)
    print(
        f"{formatted_key[0]} {formatted_key[1]}\t{' '.join(map(str, sorted_values))}")
