cmake_minimum_required(VERSION 3.5)

project(AutoLightsZenoh)

find_package(zenohc REQUIRED)
if (zenohc_FOUND)
    message(STATUS "Using ZenohC ${zenohc_VERSION}")    
endif ()

find_package(zenohcxx REQUIRED)
if (zenohcxx_FOUND)
    message(STATUS "Using ZenohC++ ${zenohcxx_VERSION}")    
endif ()

add_subdirectory(${PROJECT_SOURCE_DIR}/protobuf)

link_libraries(zenohcxx::zenohc)

add_executable(Timer src/Timer.cpp)
add_executable(FrontCamera src/FrontCamera.cpp)
add_executable(LightSensor src/LightSensor.cpp)
add_executable(HighBeams src/HighBeams.cpp)
add_executable(Config src/Config.cpp)
add_executable(ModeSelector src/ModeSelector.cpp)