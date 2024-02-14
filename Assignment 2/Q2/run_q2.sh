x
#!/bin/bash
# inhe put in the submit folder fer vhan ke submit ke jaake run as simeple as that bascially dono scripts ko is sh ko fer you run karo

LOCAL_INPUT=input.txt
HDFS_INPUT=/user/root/input.txt
HDFS_OUTPUT=/user/root/output

# Check if the local input file exists
if [ -e $LOCAL_INPUT ]; then
    # File exists, upload to HDFS
    hdfs dfs -put $LOCAL_INPUT $HDFS_INPUT

    # Specify the mapper and reducer scripts
    MAPPER_SCRIPT=mapper_q2.py
    REDUCER_SCRIPT=reducer_q2.py

    # Run the MapReduce job using mapred streaming
    mapred streaming \
        -files $MAPPER_SCRIPT,$REDUCER_SCRIPT \
        -input $HDFS_INPUT \
        -output $HDFS_OUTPUT \
        -mapper $MAPPER_SCRIPT \
        -reducer $REDUCER_SCRIPT

    # Display the contents of the output files
    hdfs dfs -cat $HDFS_OUTPUT/part-00000

    # Delete the input and output files
    hdfs dfs -rm $HDFS_INPUT
    hdfs dfs -rm -r $HDFS_OUTPUT

    echo "Hadoop job completed successfully!"
else
    # File does not exist, print a message
    echo "Input file $LOCAL_INPUT not found. Please make sure it exists in the current directory."
fi
