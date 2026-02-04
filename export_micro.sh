#!/bin/bash

# Script to export only the essential GamepadCore files to a destination directory.
# Useful for integrating the library into another project while keeping the source repo clean.

set -e

# Function to display usage information
usage() {
    echo "Usage: $0 <destination_directory>"
    echo "Example: $0 ../MyProject/libs/GamepadCore"
    exit 1
}

# Check if destination argument is provided
if [ -z "$1" ]; then
    echo "Error: No destination directory specified."
    usage
fi

DEST_DIR="$1"

echo "--- Starting export for microprocessed use ---"

# Check if we are in the correct directory (root of GamepadCore)
if [ ! -d "Source" ] || [ ! -f "CMakeLists.txt" ]; then
    echo "Error: This script must be executed at the root of the GamepadCore project."
    exit 1
fi

# Create destination directory if it doesn't exist
if [ ! -d "$DEST_DIR" ]; then
    echo "Creating destination directory: $DEST_DIR"
    mkdir -p "$DEST_DIR"
fi

# Essential items to copy
ITEMS_TO_COPY=(
    "Source"
    "CMakeLists.txt"
    "LICENSE"
)

for item in "${ITEMS_TO_COPY[@]}"; do
    if [ -e "$item" ]; then
        echo "Copying $item to $DEST_DIR..."
        cp -R "$item" "$DEST_DIR/"
    else
        echo "Warning: $item not found, skipping."
    fi
done

echo "--- Export completed! ---"
echo "Files exported to: $DEST_DIR"
ls -F "$DEST_DIR"
