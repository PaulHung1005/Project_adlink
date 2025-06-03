#!/bin/bash

# Set the directories to compare
dir1="$PWD/adlink-platforms/Platform/Ampere/ComHpcAltPkg/Override"
dir2="$PWD"

# Output file for changed files
output="changed_files.txt"
> "$output"  # Clear output file

# Loop through all files in dir1
find "$dir1" -type f | while read -r file1; do
    # Get the relative path
    rel_path="${file1#$dir1/}"
    
    file2="$dir2/$rel_path"

    # Check if file exists in dir2
    if [ -f "$file2" ]; then
        # Compare the files
        if ! cmp -s "$file1" "$file2"; then
            echo "$rel_path" >> "$output"
        fi
    fi
done

echo "Changed files list saved to $output"