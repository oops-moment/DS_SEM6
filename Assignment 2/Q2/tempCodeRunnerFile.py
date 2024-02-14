from itertools import combinations
result_dict = {}

input_lines = [
    # put some smpel alue like 1 tab 5 
    # Add more lines as needed
    "1\t2",
    "1\t3",
    "1\t4",
    "1\t5",
    "2\t3",
    "2\t4",
    "2\t5",
    "3\t4",
    "3\t5",
]

for line in input_lines:
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
    print(f"{key1} {key2}\t{','.join(map(str, sorted(mutual_friends)))}")
