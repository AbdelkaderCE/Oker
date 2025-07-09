# Python I/O and String Manipulation Benchmark
import time
import os

filename = "large_text_file.py.txt"
num_lines = 10000
content_list = []

print(f"Python Benchmark: Generating {num_lines} lines of text...")

start_time = time.time()

# Generate a large string (using a list and join is more efficient in Python)
for i in range(num_lines):
  content_list.append(f"This is line number {i} of the test file.\\n")
content = "".join(content_list)

print(f"Saving file: {filename}")
with open(filename, "w") as f:
  f.write(content)

print("Reading file back...")
with open(filename, "r") as f:
  read_content = f.read()

print("Processing file... (counting lines)")
# Use a simple and fast method to count lines
line_count = read_content.count('\\n')
print(f"Line count: {line_count}")

os.remove(filename)

end_time = time.time()
duration_ms = (end_time - start_time) * 1000

print(f"\n--- Total I/O time: {duration_ms:.2f} ms ---")
