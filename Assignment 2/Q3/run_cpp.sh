#!/bin/bash

LOCAL_INPUT=input.txt
HDFS_INPUT=/user/root/input.txt
HDFS_OUTPUT=/user/root/output

# Check if the local input file exists
if [ -e $LOCAL_INPUT ]; then
    # File exists, upload to HDFS
    hdfs dfs -put $LOCAL_INPUT $HDFS_INPUT

    # Specify the mapper and reducer scripts for the first stage
    MAPPER_SCRIPT0=mapper0.py
    REDUCER_SCRIPT0=reducer0.py

    # Run the first MapReduce job using mapred streaming
    mapred streaming \
        -files $MAPPER_SCRIPT0,$REDUCER_SCRIPT0 \
        -input $HDFS_INPUT \
        -output $HDFS_OUTPUT \
        -mapper $MAPPER_SCRIPT0 \
        -reducer $REDUCER_SCRIPT0 \

    # Specify the mapper and reducer scripts for the second stage
    MAPPER_SCRIPT1=mapper1.py
    REDUCER_SCRIPT1=reducer1.py

    # Run the second MapReduce job using mapred streaming
    mapred streaming \
        -files $MAPPER_SCRIPT1,$REDUCER_SCRIPT1 \
        -input $HDFS_OUTPUT/part-* \
        -output $HDFS_OUTPUT/result \
        -mapper $MAPPER_SCRIPT1 \
        -reducer $REDUCER_SCRIPT1 \
    
    # Run the second MapReduce job using mapred streaming
    mapred streaming \
        -files $MAPPER_SCRIPT1,$REDUCER_SCRIPT1 \
        -input $HDFS_OUTPUT/result/part-* \
        -output $HDFS_OUTPUT/result1 \
        -mapper $MAPPER_SCRIPT1 \
        -reducer $REDUCER_SCRIPT1 \

    # Display the contents of the output files
    hdfs dfs -cat $HDFS_OUTPUT/result1/part-*
    

    # Delete the input and output files
    hdfs dfs -rm $HDFS_INPUT
    hdfs dfs -rm -r $HDFS_OUTPUT

    echo "Hadoop job completed successfully!"
else
    # File does not exist, print a message
    echo "Input file $LOCAL_INPUT not found. Please make sure it exists in the current directory."
fi
