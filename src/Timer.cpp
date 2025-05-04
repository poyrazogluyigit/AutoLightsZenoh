#include <functional>
#include <chrono>
#include <atomic>
#include <memory>
#include <thread>
#include "zenoh.hxx"

typedef std::chrono::milliseconds Interval;
typedef std::function<void(void)> Timeout;

struct TimerThread {
    std::thread thread;
    std::atomic<bool> running;
    Timeout timeout;

    ~TimerThread() {
        this->running = false;
        if (this->thread.joinable()) this->thread.join();
    }
};

std::shared_ptr<TimerThread> start(const Interval &interval, const Timeout &timeout){
    auto th = std::make_shared<TimerThread>();
    th->running = true;
    th->timeout = timeout;
    th->thread = std::thread([th, interval]()
    {
        std::this_thread::sleep_for(interval);
        if (th->running) th->timeout();
    });
    return th;
}

void stop(std::shared_ptr<TimerThread> th){
    if (th) {
        th->running = false;
        if (th->thread.joinable()) th->thread.join();
    }
}

int main(){

    // zenoh::Config config = zenoh::Config::create_default();
    // auto session = zenoh::Session::open(std::move(config));
    // auto callback = [](zenoh::Sample &sample) {};
    // auto timerSession = session.declare_subscriber("autoLights/timer", callback, zenoh::closures::none);

    auto timer = start(std::chrono::milliseconds(10000), [](){std::cout << "timeout" << std::endl;});
    std::this_thread::sleep_for(std::chrono::milliseconds(20000));
    std::cout << "wakeup" << std::endl;

    return 0;
}