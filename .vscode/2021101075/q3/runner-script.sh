#!/bin/bash

LOCAL_INPUT=input.txt
HDFS_INPUT=/user/root/input.txt
BASE_OUTPUT_DIR=/user/root/output

# Check if the local input file exists
if [ -e $LOCAL_INPUT ]; then

    # File exists, upload to HDFS
    hdfs dfs -mkdir -p /user/root
    hdfs dfs -put $LOCAL_INPUT $HDFS_INPUT

    # Specify the mapper and reducer scripts for the first stage
    MAPPER_SCRIPT0=mapper0.py
    REDUCER_SCRIPT0=reducer0.py

    # Run the first MapReduce job using mapred streaming
    mapred streaming \
        -files $MAPPER_SCRIPT0,$REDUCER_SCRIPT0 \
        -input $HDFS_INPUT \
        -output $BASE_OUTPUT_DIR/output0 \
        -mapper $MAPPER_SCRIPT0 \
        -reducer $REDUCER_SCRIPT0 \
        -numReduceTasks 2

    # Specify the mapper and reducer scripts for the second stage
    MAPPER_SCRIPT1=mapper1.py
    REDUCER_SCRIPT1=reducer1.py

    # Get the number of iterations based on the number of lines in input.txt
    NUM_ITERATIONS=$(hdfs dfs -cat $HDFS_INPUT | wc -l)
    # NUM_ITERATIONS= 2
    
    echo "Number of iterations: $NUM_ITERATIONS"
    for ((i=1; i<=$NUM_ITERATIONS; i++)); do
        # Run the second MapReduce job using mapred streaming
        mapred streaming \
            -files $MAPPER_SCRIPT1,$REDUCER_SCRIPT1 \
            -input $BASE_OUTPUT_DIR/output$((i-1))/part-* \
            -output $BASE_OUTPUT_DIR/output$i \
            -mapper $MAPPER_SCRIPT1 \
            -reducer $REDUCER_SCRIPT1 \
            -numReduceTasks 2
        
    done

    # send it to final output use one more mapper and reducer
    MAPPER_SCRIPT2=mapper2.py
    REDUCER_SCRIPT2=reducer2.py

    # Run the third MapReduce job using mapred streaming
    mapred streaming \
        -files $MAPPER_SCRIPT2,$REDUCER_SCRIPT2 \
        -input $BASE_OUTPUT_DIR/output$NUM_ITERATIONS/part-* \
        -output $BASE_OUTPUT_DIR/final_output \
        -mapper $MAPPER_SCRIPT2 \
        -reducer $REDUCER_SCRIPT2 \
        -numReduceTasks 1

    # Display the final contents of the output files
    hdfs dfs -cat $BASE_OUTPUT_DIR/final_output/part-00000

    # Delete the input and output files
    hdfs dfs -rm $HDFS_INPUT
    hdfs dfs -rm -r $BASE_OUTPUT_DIR

    echo "Hadoop job completed successfully!"
else
    # File does not exist, print a message
    echo "Input file $LOCAL_INPUT not found. Please make sure it exists in the current directory."
fi
