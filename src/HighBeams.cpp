#include <functional>
#include <thread>
#include "zenoh.hxx"

using namespace std::chrono_literals;

int main(){

    bool highBeams = false;

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    auto callback = [&](zenoh::Sample &sample) {
        std::cout << "High beams " << sample.get_payload().as_string() << std::endl;
        highBeams = sample.get_payload().as_string() == "turn on";
        session.put("autoLights/highBeams/reply", highBeams ? "on" : "off");
    };

    auto highBeamsSession = session.declare_subscriber("autoLights/highBeams", callback, zenoh::closures::none);

    while(true) {
        std::this_thread::sleep_for(100ms);
    }
    return 0;
}