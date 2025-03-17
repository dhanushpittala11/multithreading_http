# Multithreaded HTTP Request Scheduler

This project implements a multithreaded task scheduler in C++ to simulate a web server processing HTTP requests concurrently.

## Features
- **Concurrency**: Processes multiple HTTP requests concurrently using worker threads.
- **Thread Safety**: Ensures thread-safe access to the task queue using mutexes and condition variables.
- **Real-World Simulation**: Simulates HTTP requests with random delays to mimic real-world workloads.

## How to Run
1. Compile the code:
   ```bash
   g++ http_scheduler.cpp -o http_scheduler -pthread

2. Run the Executable:
   ```bash
   ./http_scheduler
