 #!/bin/bash

if [[ $#  -ne 1 ]]; then
	echo "USAGE: run <input file>"
	echo         "<input file> = file to convert into floppies"
	echo         "NOTE: After sharding original file into floppies,"
	echo         "      you will be asked to enter required # of floppies."
	exit
fi

INPUT_FILE=$1
OUTPUT_FILE=final.file

# clean up local directory
rm -rf floppy.*
rm -rf $OUTPUT_FILE

# create binary
make clean && make

# sender side testing
./exablox -s $INPUT_FILE 

# receiver side testing
echo "Please provide number of floppies required for file reconstruction"
read USER_INPUT 
LAST_ELEMENT=($USER_INPUT - 1)
for i in $(seq 1 $USER_INPUT); do
	if [ $i -eq $LAST_ELEMENT ]; then
		FLOPPY_LIST+="floppy.$i"
	else
		FLOPPY_LIST+="floppy.$i,"
	fi
done

./exablox -r $FLOPPY_LIST

# external utilities for verification
echo -n "$INPUT_FILE:  "
cat $INPUT_FILE | openssl dgst -sha256

echo -n "$OUTPUT_FILE: "
cat $OUTPUT_FILE | openssl dgst -sha256
