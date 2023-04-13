cmake_minimum_required(VERSION 3.5)

project(opencv-mouse-click LANGUAGES CXX)

find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)

find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIR})

add_executable(opencv-mouse-click main.cpp)

target_link_libraries(opencv-mouse-click PRIVATE ${OpenCV_LIBS} Qt5::Widgets Qt5::Gui yaml-cpp)
