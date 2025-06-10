#include "Threading.h"

Threading::Threading()
{}

void Threading::CreateThreadPool(size_t Threads)
{
    for (size_t Index = 0; Index < Threads; Index++)
    {
        this->WorkerThreads.emplace_back([this]{
            while (true)
            {
                std::function<void()> Task;
                {
                    std::unique_lock<std::mutex> Lock(this->WorkerConditionalMutex);
                    this->WorkerVariableCondition.wait(Lock,[this]{return this->WorkerStop || !this->WorkerTasks.empty();});
                    if (this->WorkerStop && this->WorkerTasks.empty()) return;
                    Task = std::move(this->WorkerTasks.front());
                    this->WorkerTasks.pop();
                }
                Task();
            }
        });
    }
}

void Threading::CleanThreadPool()
{
    {
        std::lock_guard<std::mutex> Lock(this->WorkerConditionalMutex);
        this->WorkerStop = true;
    }
    this->WorkerVariableCondition.notify_all();
    for (std::thread &Worker: this->WorkerThreads)
    {Worker.join();}
}

void Threading::EnqueueThreadPool(std::function<void()> Task)
{
    {
        std::lock_guard<std::mutex> Lock(this->WorkerConditionalMutex);
        this->WorkerTasks.push(std::move(Task));
    }
    this->WorkerVariableCondition.notify_one();
}