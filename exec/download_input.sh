#!/bin/bash

mkdir -p data
if [ ! -f "data/input.txt" ]; then
    echo "downloading dataset..."
    cd exec/script_download_input/
    uv run main.py
    cd ../../
fi