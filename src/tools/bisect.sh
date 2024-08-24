#!/bin/bash

echo "Running omni-c on each file individually to isolate some failures for debugging"

for file in "$@"; do  # Iterate over all command-line arguments
    echo "Processing $file..."
    ./omni-c generate-library --dump-symbol-table=false --output-file=/tmp/foo.c "$file"
    if [ $? -ne 0 ]; then
        echo "Error processing $file"
        exit 1 
    fi
done

echo "All files processed successfully"
