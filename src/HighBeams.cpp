#include <functional>
#include <thread>
#include "zenoh.hxx"

using namespace std::chrono_literals;

int main(){

    bool highBeams = 0;

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    auto callback = [&highBeams](zenoh::Sample &sample) {
        std::cout << "High beams toggled " << (0 ? "off" : "on") << std::endl;
        highBeams = !highBeams;
    };

    auto highBeamsSession = session.declare_subscriber("autoLights/highBeams", callback, zenoh::closures::none);

    while(true) {
        std::this_thread::sleep_for(100ms);
    }
    return 0;
}