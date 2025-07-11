# Python Fibonacci Benchmark
# This script does the same calculation as the Oker version for comparison.

import time

def fib(n):
    if n <= 1:
        return n
    return fib(n - 1) + fib(n - 2)

print("Calculating fib(35) in Python...")

start_time = time.time()
result = fib(35)
end_time = time.time()

duration_ms = (end_time - start_time) * 1000

print(f"Result: {result}")
print(f"\n--- Execution time: {duration_ms:.2f} ms ---")
