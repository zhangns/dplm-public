#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H
// http://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads/19659736#19659736
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    explicit Semaphore(int count);
    void release();
    void acquire();
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

inline Semaphore::Semaphore(int count): count(count) {}

inline void Semaphore::release() {
    std::unique_lock<std::mutex> lock(mtx);
    ++count;
    cv.notify_one();
}

inline void Semaphore::acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    while (count == 0)
        cv.wait(lock);
    --count;
}

#endif
