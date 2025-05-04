#include <functional>
#include <thread>
#include "zenoh.hxx"

using namespace std::chrono_literals;

int main(){

    int timerDelay = 100;

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    // this is going to cause a problem
    auto callback = [&session, &timerDelay](zenoh::Sample &sample) {
        session.put("autoLights/config/reply", std::to_string(timerDelay));
    };

    auto configSession = session.declare_subscriber("autoLights/config", callback, zenoh::closures::none);

    while(true) {
        std::this_thread::sleep_for(100ms);
    }
    return 0;
}