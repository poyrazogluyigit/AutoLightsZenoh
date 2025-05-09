#include <functional>
#include <chrono>
#include <thread>
#include "zenoh.hxx"

using namespace std::chrono_literals;

bool highLight = false;

int main(){

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));
    auto lightSensorSession = session.declare_publisher("autoLights/lightSensor");

    std::this_thread::sleep_for(20s);
    lightSensorSession.put("Low light");
    std::cout << "Low light" << std::endl;

    while(true) {
        std::this_thread::sleep_for(20s);
        // lightSensorSession.put("Low light");
        // std::cout << "Low light" << std::endl;
        // std::this_thread::sleep_for(20s);
        // lightSensorSession.put("High light") ;
        // std::cout << "High light" << std::endl;
    }
    return 0;
}