import requests
import gzip
import json
import io
import os

OUTPUT_DIRECTORY = "../../data/"
FILE_NAME = "input.txt"
TARGET_SIZE = 1024 * 1024 * 1024  # 1 GB
URLS = [
    "https://huggingface.co/datasets/allenai/c4/resolve/main/en/c4-train.00000-of-01024.json.gz",
    "https://huggingface.co/datasets/allenai/c4/resolve/main/en/c4-train.00001-of-01024.json.gz"
    "https://huggingface.co/datasets/allenai/c4/resolve/main/en/c4-train.00002-of-01024.json.gz"
]

def download_clean_text():
    if not os.path.exists(OUTPUT_DIRECTORY):
        os.makedirs(OUTPUT_DIRECTORY)
    full_path = os.path.join(OUTPUT_DIRECTORY, FILE_NAME)
    bytes_written = 0
    try:
        with open(full_path, "wb") as out_file:
            for url in URLS:
                if bytes_written >= TARGET_SIZE:
                    break
                with requests.get(url, stream=True, timeout=120) as r:
                    r.raise_for_status()
                    with gzip.GzipFile(fileobj=r.raw) as decompressor:
                        text_stream = io.TextIOWrapper(decompressor, encoding='utf-8')
                        for line in text_stream:
                            try:
                                data = json.loads(line)
                                content = data.get("text", "")
                                if content:
                                    encoded_content = (content + "\n").encode('utf-8')
                                    out_file.write(encoded_content)
                                    bytes_written += len(encoded_content)
                                if bytes_written >= TARGET_SIZE:
                                    return
                            except Exception:
                                continue
    except Exception as e:
        print(f"\n Errore critico: {e}")

if __name__ == "__main__":
    download_clean_text()