#include "../include/threadpool.h"

Thread_pool_t::Thread_pool_t() : canAccess(false), poolTerminated(false) {
    std::size_t numThreads = std::thread::hardware_concurrency();
    construct(numThreads);
}

Thread_pool_t::Thread_pool_t(std::size_t numThreads) : canAccess(false), poolTerminated(false) {
    construct(numThreads);
}

void
Thread_pool_t::enqueue(task_t&& task){
    std::unique_lock<std::mutex> lock(queueMtx);
    tasks.emplace(std::move(task));
    lock.unlock();
    cv.notify_one();
}

Thread_pool_t::~Thread_pool_t() {
    if (!poolTerminated){
        shutdown();
    }
}

void
Thread_pool_t::executeTask(optional_task_t task){
    if (task) {
        (*task)();
    }
}

void
Thread_pool_t::construct(std::size_t numThreads){

    poolTerminated = false;

    for (std::size_t i = 0; i < numThreads; i++){
        workerThreads.emplace_back([this] () -> void{

            for (;;){
                std::unique_lock<std::mutex> lock(queueMtx);
                cv.wait(lock, [this] () -> bool{
                    return canRunTask();
                });

                if (canTerminateTask()){
                    return;
                }

                optional_task_t task = std::move(tasks.front());
                tasks.pop();
                lock.unlock();
                if (task){
                    executeTask(task);
                }
            }
        });
    }
}

bool
Thread_pool_t::canRunTask() const {
    return canAccess || !tasks.empty();
}

bool
Thread_pool_t::canTerminateTask() const {
    return canAccess && tasks.empty();
}

void
Thread_pool_t::shutdown(){
    
    std::unique_lock<std::mutex> lock(queueMtx);
    canAccess = true;
    lock.unlock();
    cv.notify_all();
    for (std::thread& t : workerThreads ){
        t.join();
    }
    poolTerminated = true;
}