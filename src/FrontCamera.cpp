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

    int ctr = 0;
    int passedCtr = 0;
    bool carsExist = false;
    while(true) {
        std::this_thread::sleep_for(100ms);
        ctr = (ctr + 1) % 50; 
        passedCtr = (passedCtr + 1) % 50;
        if (ctr == 0) {
            timerSession.put("Car detected");
            carsExist = true;
        }
        if (carsExist && passedCtr == 20) timerSession.put("Car passed");


    }
    return 0;
}