import argparse
import gzip
import json
import os
import sys
import requests

# Configuration
OUTPUT_DIR = "../../data/"
OUTPUT_FILE = "input.txt"
# C4 dataset (Colossal Clean Crawled Corpus) - huge, high quality text
BASE_URL = "https://huggingface.co/datasets/allenai/c4/resolve/main/en/c4-train.{:05d}-of-01024.json.gz"
MAX_FILES = 1024

def parse_size(size_str):
    """Parses size string (e.g., '100', '100KB', '1.5 GB') into bytes."""
    size_str = str(size_str).strip().upper()
    if size_str.replace('.', '', 1).isdigit():
         return int(float(size_str) * 1024 * 1024)
    import re
    match = re.match(r'^([\d.]+)\s*([A-Z]+)$', size_str)
    if not match:
        raise argparse.ArgumentTypeError(f"Invalid size format: {size_str}")
    value = float(match.group(1))
    unit = match.group(2)
    multipliers = {
        'B': 1,
        'KB': 1024,
        'MB': 1024 * 1024,
        'GB': 1024 * 1024 * 1024
    }
    if unit not in multipliers:
        raise argparse.ArgumentTypeError(f"Unknown unit: {unit}. Supported: B, KB, MB, GB")
    return int(value * multipliers[unit])

def get_args():
    parser = argparse.ArgumentParser(description="Download text data for analysis.")
    parser.add_argument("target_size", type=parse_size, nargs="?", default="1024", help="Target size (e.g., 500KB, 100, 1.5GB). Default unit is MB.")
    return parser.parse_args()

def main():
    args = get_args()
    target_bytes = args.target_size
    
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)
    
    output_path = os.path.join(OUTPUT_DIR, OUTPUT_FILE)
    current_bytes = 0
    print(f"Target size: {target_bytes / (1024 * 1024):.2f} MB")
    print("Downloading...")
    try:
        with open(output_path, "wb") as f_out:
            for i in range(MAX_FILES):
                if current_bytes >= target_bytes:
                    break
                url = BASE_URL.format(i)
                try:
                    with requests.get(url, stream=True, timeout=60) as r:
                        r.raise_for_status()
                        # Streaming decompression
                        with gzip.GzipFile(fileobj=r.raw) as f_in:
                            for line in f_in:
                                try:
                                    data = json.loads(line)
                                    text = data.get("text", "")
                                    if not text:
                                        continue
                                    encoded_text = (text + "\n").encode("utf-8")
                                    f_out.write(encoded_text)
                                    current_bytes += len(encoded_text)
                                    if current_bytes >= target_bytes:
                                        break
                                except (json.JSONDecodeError, UnicodeDecodeError):
                                    continue
                except Exception as e:
                    print(f"\nWarning: Skipping chunk {i} due to error: {e}", file=sys.stderr)
                    continue
        print(f"Output file: {output_path}")
        print(f"Final size: {current_bytes / (1024*1024):.2f} MB")
        
    except KeyboardInterrupt:
        print(f"\n\nInterrupted. Saved so far: {current_bytes / (1024*1024):.2f} MB")
    except Exception as e:
        print(f"\nCritical Error: {e}", file=sys.stderr)

if __name__ == "__main__":
    main()
