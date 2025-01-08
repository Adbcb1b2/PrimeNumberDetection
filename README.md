# Multithreaded Prime Number Detector in C

This program uses multithreading (via POSIX threads) to detect prime numbers from a file containing integers. It divides the work across a user-specified number of threads, processes each section of the data in parallel, and writes the detected prime numbers to an output file.

---

## Features

- **Multithreaded Execution**: Efficiently divides the workload among multiple threads.
- **Prime Detection**: Identifies prime numbers from a list of integers in a file.
- **Custom Thread Count**: Allows the user to specify the number of threads via a command-line argument.
- **File Handling**: Reads integers from an input file and writes detected prime numbers to an output file.
- **Scalability**: Handles uneven data distribution by balancing computation across threads.

---

## Prerequisites

To build and run this program, you need:
- A C compiler that supports POSIX threads (e.g., GCC).
- A file containing integers, one per line.

---
