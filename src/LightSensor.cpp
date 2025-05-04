#include <functional>
#include <chrono>
#include <thread>
#include "zenoh.hxx"

using namespace std::chrono_literals;

int highLight = 0;

int main(){

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));
    auto lightSensorSession = session.declare_publisher("autoLights/lightSensor");

    int ctr = 0;
    while(true) {
        std::this_thread::sleep_for(100ms);
        ctr = (ctr + 1) % 100; 
        if (ctr == 0) highLight = 0;
        highLight ? lightSensorSession.put("Low light") 
                    : lightSensorSession.put("High light");
    }
    
    return 0;
}