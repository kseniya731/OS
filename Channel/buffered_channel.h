#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size_(size > 0 ? size : 0), closed_(false) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (closed_) {
            throw std::runtime_error("Can't send to the closed channel");
        }

        send_cv_.wait(lock, [this]() {
            return closed_ || queue_.size() < buffer_size_;
            });

        if (closed_) {
            throw std::runtime_error("Can't send to the closed channel");
        }

        queue_.push(std::move(value));
        recv_cv_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);
    
        recv_cv_.wait(lock, [this]() {
            return closed_ || !queue_.empty();
            });
    
        if (closed_ && queue_.empty()) {
            return { T(), false };
        }
    
        T value = std::move(queue_.front());
        queue_.pop();
        send_cv_.notify_one();
        return { std::move(value), true };
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mutex_);
        closed_ = true;
        send_cv_.notify_all();
        recv_cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    const size_t buffer_size_;
    bool closed_;
    std::mutex mutex_;
    std::condition_variable send_cv_;
    std::condition_variable recv_cv_;
};


#endif // BUFFERED_CHANNEL_H_
