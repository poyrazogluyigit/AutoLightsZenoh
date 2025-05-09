#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include "zenoh.hxx"

/*
    TODO
    [x] Finish 'toggle beams' parts
    [x] Finish timer zenoh integration
    [x] Double check topics
    [x] Put state logic in a function that fires only when a message is received
        - [x] Probably need to change lambdas into full functions
    [ ] Testing
    [ ] Check Arrowhead example
*/

struct LightSensor {
public:
    enum State {Low, High};
    void setState(State state, std::function<void()> func){
        std::cout << "Changing LightSensor state to " << (state == Low ? "Low" : "High") << std::endl;
        this->state = state;
        func();
    }
    bool isLow(){
        return this->state == State::Low;
    }
private:
    State state{High};
};

struct FrontCamera {
enum State {Detected, NotDetected};
void setState(std::string message, std::function<void()> func){
    if (message == "Car detected") {
        std::cout << "Car detected" << std::endl;
        this->state = Detected;
        func();
    }
    else if (message == "Car passed") {
        std::cout << "Car passed" << std::endl;
        this->state = NotDetected;
        func();
    }
}
bool isCarDetected() {
    return this->state == State::Detected;
}
private:
    State state{NotDetected};
};

struct ControlMode{
    enum Mode {Automatic, Manual};

    void changeMode(Mode mode, std::function<void()> func){
        std::cout << "Changing mode to " << (mode == Automatic ? "Automatic" : "Manual") << std::endl;
        this->mode = mode;
        func();
    }
    bool isManual(){return mode == Manual;}
    bool isAutomatic(){return mode == Automatic;}
private:
    Mode mode{Manual};
};

struct HighBeamsRelay {
public:
    enum State {On, Off};
    void setState(State state, std::function<void()> func){
        std::cout << "Changing High Beams Relay state to " << (state == On ? "On" : "Off") << std::endl;
        this->state = state;
        func();
    }
    bool isOn(){
        return this->state == State::On;
    }
private:
    State state{Off};
};

struct Timer {
    bool isArmed(){return armed;}
    bool isTimeout(){return timeout;}
    void setArmed(std::string message, std::function<void()> func){
        if (message == "Armed") {
            std::cout << "Arming timer" << std::endl;
            armed = true;
            func();
        }
        else if (message == "Disarmed") {
            std::cout << "Timer disarmed/cancelled" << std::endl;
            armed = false;
        }
    }
    void setTimeout(std::string message, std::function<void()> func){
        if (message == "Timeout") {
            std::cout << "Timer timeout" << std::endl;
            timeout = true;
            func();
            timeout = false;
            armed = false;
        }
    }
private:
    bool armed = false;
    bool timeout = false;
};

struct Module {
    enum State {Uninitialized, Operational};
    void setState(State state, std::function<void()> func){
        this->state = state;
        func();
    }
    bool isOperational(){return state == Operational;}
    bool isUninitialized(){return state == Uninitialized;}
private:
    State state{Uninitialized};
};

int main(){

    zenoh::Config config = zenoh::Config::create_default();
    auto session = zenoh::Session::open(std::move(config));

    int timerDelay = 0;
    ControlMode mode;
    Module module;

    Timer timer;
    LightSensor lightSensor;
    FrontCamera frontCamera;
    HighBeamsRelay highBeamsRelay;

    bool lowLight = false;

    bool shifterToggle = false;
    bool moduleTerminate = false;

    auto toggleBeams = [&](){highBeamsRelay.isOn()
                                ? session.put("autoLights/highBeams", "turn off")
                                : session.put("autoLights/highBeams", "turn on");
                                shifterToggle = false;};

    std::function<void()> stateLogic = [&](){
        if (module.isOperational()){
            std::cout << "Operational, ";
            if (mode.isManual()){
                std::cout << "Manual, ";
                if (!timer.isArmed()){
                    std::cout << "Not armed, ";
                    if (lightSensor.isLow()) {
                        std::cout << "Low light, ";
                        if (!highBeamsRelay.isOn()) {
                            std::cout << "High beams off, ";
                            session.put("autoLights/timer", std::to_string(timerDelay));
                        }
                    }
                }
                else if (timer.isArmed()) {
                    std::cout << "Armed, ";
                    if (!lightSensor.isLow()) {
                        std::cout << "High light, ";
                        session.put("autoLights/timer", "cancel");
                    }
                    if (frontCamera.isCarDetected()) {
                        std::cout << "Car detected, ";
                        session.put("autoLights/timer", "cancel");
                    }
                }
                if (timer.isTimeout()) {
                    std::cout << "Timeout, ";
                    mode.changeMode(mode.Automatic, [](){});
                    session.put("autoLights/highBeams", "turn on");
                    lowLight = true;
                }
                if (shifterToggle) {
                    std::cout << "Shifter toggle, ";
                    session.put("autoLights/timer", "cancel");
                    toggleBeams();
                }
            }
            else if (mode.isAutomatic()) {
                std::cout << "Automatic, ";
                if (highBeamsRelay.isOn()) {
                    std::cout << "High beams on, ";
                    if (!lightSensor.isLow()) {
                        std::cout << "High light, ";
                        if (!timer.isArmed()) {
                            std::cout << "Timer not armed, ";
                            session.put("autoLights/timer", std::to_string(timerDelay));
                        }
                    }
                    if (lightSensor.isLow()) {
                        std::cout << "Low light, ";
                        session.put("autoLights/timer", "cancel");
                    }
                    if (frontCamera.isCarDetected()) {
                        std::cout << "Car detected, ";
                        session.put("autoLights/highBeams", "turn off");
                    }
                    if (shifterToggle) {
                        std::cout << "Shifter toggled, ";
                        session.put("autoLights/highBeams", "turn off");
                        mode.changeMode(mode.Manual, [](){});
                        session.put("autoLights/timer", "cancel");
                    }
                }
                if (lowLight) {
                    std::cout << "Low light (var), ";
                    if (timer.isTimeout()) {
                        std::cout << "Timeout, ";
                        lowLight = !lowLight;
                        toggleBeams();
                    }
                }
                if (!lowLight) {
                    std::cout << "High light(var), ";
                    if (timer.isTimeout()) {
                        std::cout << "Timeout, ";
                        lowLight = !lowLight;
                        toggleBeams();
                    }
                }
                if (moduleTerminate) {
                    std::cout << "Terminate, ";
                    mode.changeMode(mode.Manual, [](){});
                    session.put("autoLights/timer", "cancel");
                }
            }
            else exit(1);
            std::cout << std::endl;
        }
        else {
            if (shifterToggle) {
                session.put("autoLights/timer", "cancel");
                toggleBeams();
            }
        }
    };

    // listeners
    auto camCb = [&](const zenoh::Sample &sample){
        std::string message = sample.get_payload().as_string();
        frontCamera.setState(message, stateLogic);
    };

    auto camSub = session.declare_subscriber("autoLights/frontCamera", camCb, zenoh::closures::none);

    auto timCb = [&](const zenoh::Sample &sample){
        std::string message = sample.get_payload().as_string();
        timer.setArmed(message, [](){});
        timer.setTimeout(message, stateLogic);   
    };
    auto timSub = session.declare_subscriber("autoLights/timer/reply", timCb, zenoh::closures::none);

    auto lightCb = [&](const zenoh::Sample &sample){
        std::string message = sample.get_payload().as_string();
        lightSensor.setState(message == "Low light" ? LightSensor::State::Low :
                                                    LightSensor::State::High, stateLogic);
    };
    auto lightSub = session.declare_subscriber("autoLights/lightSensor", lightCb, zenoh::closures::none);

    auto highBeamsCB = [&](const zenoh::Sample &sample){
        HighBeamsRelay::State state = sample.get_payload().as_string() == "on" ? HighBeamsRelay::State::On :
                                                                            HighBeamsRelay::State::Off;
        highBeamsRelay.setState(state, [](){});
    };
    auto highBeamsSub = session.declare_subscriber("autoLights/highBeams/reply", highBeamsCB, zenoh::closures::none);

    auto confCb = [&](const zenoh::Sample &sample){
        timerDelay = std::stoi(sample.get_payload().as_string());
        std::cout << "Received delay " << timerDelay << std::endl;
        module.setState(Module::State::Operational, [](){});
    };
    auto confSub = session.declare_subscriber("autoLights/config/reply", confCb, zenoh::closures::none);

    session.put("autoLights/config", "getLightShifterDelay()");

    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}