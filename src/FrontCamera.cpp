#include <functional>
#include <chrono>
#include <thread>
#include "zenoh.hxx"

using namespace std::chrono_literals;

int carDetected = 0;
int carPassed = 0;

int main(){

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));
    auto timerSession = session.declare_publisher("autoLights/frontCamera");

    std::this_thread::sleep_for(50s);
    timerSession.put("Car detected");
    std::cout << "Car detected" << std::endl;


    while(true) {
        std::this_thread::sleep_for(30s);
        // timerSession.put("Car detected");
        // std::cout << "Car detected" << std::endl;
        // std::this_thread::sleep_for(5s);
        // timerSession.put("Car passed");
        // std::cout << "Car passed" << std::endl;
    }
    return 0;
}