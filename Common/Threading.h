#ifndef __THREADING_HEADER__
#define __THREADING_HEADER__

#include <vector>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

class Threading
{
    public:
        Threading();
        void CreateThreadPool(size_t Threads);
        void EnqueueThreadPool(std::function<void()> ThreadTask);
        void CleanThreadPool();
    private:
        bool                                WorkerStop;
        std::vector<std::thread>            WorkerThreads;
        std::queue<std::function<void()>>   WorkerTasks;
        std::mutex                          WorkerConditionalMutex;
        std::condition_variable             WorkerVariableCondition;
};

#endif