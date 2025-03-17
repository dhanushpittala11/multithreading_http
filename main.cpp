#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <random>
#include <string>

class TaskScheduler {
private:
    std::queue<std::function<void()>> taskQueue; // Queue to store tasks
    std::vector<std::thread> workers;           // Worker threads
    std::mutex queueMutex;                      // Mutex for thread-safe access to the task queue
    std::condition_variable condition;          // Condition variable for task notification
    bool stop;                                  // Flag to stop the scheduler

public:
    TaskScheduler(size_t numThreads) : stop(false) {
        // Create worker threads
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    // Lock the queue and wait for a task
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !taskQueue.empty(); });

                        // If the scheduler is stopped and the queue is empty, exit
                        if (stop && taskQueue.empty()) {
                            return;
                        }

                        // Get the next task from the queue
                        task = taskQueue.front();
                        taskQueue.pop();
                    }

                    // Execute the task
                    task();
                }
            });
        }
    }

    // Add a task to the queue
    void schedule(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            taskQueue.push(task);
        }
        condition.notify_one(); // Notify one worker thread
    }

    // Stop the scheduler and join all threads
    void stopScheduler() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all(); // Notify all worker threads

        // Join all threads
        for (std::thread& worker : workers) {
            worker.join();
        }
    }
};

// Simulate an HTTP request
void processHTTPRequest(const std::string& request) {
    std::cout << "Processing request: " << request << " on thread " << std::this_thread::get_id() << std::endl;

    // Simulate processing time (e.g., fetching data, processing data, saving data)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(500, 2000); // Random delay between 500ms and 2000ms
    std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));

    std::cout << "Completed request: " << request << std::endl;
}

int main() {
    // Create a task scheduler with 4 worker threads
    TaskScheduler scheduler(4);

    // Simulate incoming HTTP requests
    std::vector<std::string> requests = {
        "GET /api/data",
        "POST /api/upload",
        "PUT /api/update",
        "DELETE /api/delete",
        "GET /api/users",
        "POST /api/login",
        "PUT /api/settings",
        "DELETE /api/logout",
        "GET /api/products",
        "POST /api/checkout"
    };

    // Schedule tasks for each request
    for (const auto& request : requests) {
        scheduler.schedule([request] { processHTTPRequest(request); });
    }

    // Wait for all tasks to complete
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop the scheduler
    scheduler.stopScheduler();

    return 0;
}