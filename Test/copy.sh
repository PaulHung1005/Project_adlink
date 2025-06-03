#!/bin/bash

dir1="$PWD/adlink-platforms/Platform/Ampere/ComHpcAltPkg/Override"
dir2="$PWD"
input="changed_files.txt"

# Validate input file
if [ ! -f "$input" ]; then
    echo "ERROR: Input file '$input' not found!"
    exit 1
fi

while IFS= read -r rel_path; do
    # Clean path: remove carriage returns and trim
    rel_path="$(echo "$rel_path" | tr -d '\r' | xargs)"
    [ -z "$rel_path" ] && continue

    src_file="$dir1/$rel_path"
    dest_file="$dir2/$rel_path"

    # Check if source file exists
    if [ ! -f "$src_file" ]; then
        echo "ERROR: Source file not found -> $src_file"
        continue
    fi

    echo "Replacing: $dest_file"
    mkdir -p "$(dirname "$dest_file")"
    cp -f "$src_file" "$dest_file"
done < "$input"

echo "All existing files copied from $dir1 to $dir2"
