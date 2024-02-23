#!/bin/bash

# Check if the correct number of arguments are provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <LOCAL_INPUT_DIR> <HDFS_INPUT_DIR> <HDFS_OUTPUT_DIR>"
    exit 1
fi

# Assign arguments to variables
LOCAL_INPUT_DIR=$1
HDFS_INPUT_DIR=$2
HDFS_OUTPUT_DIR=$3
HDFS_TEMP_DIR=/helper

# Function to create HDFS directory if it doesn't exist
create_hdfs_directory() {
    local hdfs_dir=$1
    hdfs dfs -test -e "$hdfs_dir"
    if [ $? -ne 0 ]; then
        hdfs dfs -mkdir -p "$hdfs_dir"
    fi
}

# Create HDFS input and output directories if they don't exist
create_hdfs_directory "$HDFS_INPUT_DIR"

# Upload local input files to HDFS input directory
hdfs dfs -put "$LOCAL_INPUT_DIR" "$HDFS_INPUT_DIR/"


# Specify the mapper and reducer scripts for the first stage
MAPPER_SCRIPT0=mapper0.py
REDUCER_SCRIPT0=reducer0.py

# Run the first MapReduce job using mapred streaming
mapred streaming \
    -files "$MAPPER_SCRIPT0","$REDUCER_SCRIPT0" \
    -input "$HDFS_INPUT_DIR" \
    -output "$HDFS_TEMP_DIR" \
    -mapper "$MAPPER_SCRIPT0" \
    -reducer "$REDUCER_SCRIPT0" \
    -numReduceTasks 3

# Specify the mapper and reducer scripts for the second stage
MAPPER_SCRIPT1=mapper1.py
REDUCER_SCRIPT1=reducer1.py

# Run the second MapReduce job using mapred streaming
mapred streaming \
    -files "$MAPPER_SCRIPT1","$REDUCER_SCRIPT1" \
    -input "$HDFS_TEMP_DIR/part-*" \
    -output "$HDFS_OUTPUT_DIR" \
    -mapper "$MAPPER_SCRIPT1" \
    -reducer "$REDUCER_SCRIPT1" \
    -numReduceTasks 3

# Display the contents of the output files
hdfs dfs -cat "$HDFS_OUTPUT_DIR/part-*" | sort

# Delete the input and output files
hdfs dfs -rm -r "$HDFS_INPUT_DIR"
hdfs dfs -rm -r "$HDFS_OUTPUT_DIR"
hdfs dfs -rm -r "$HDFS_TEMP_DIR"

echo "Hadoop job completed successfully!"
