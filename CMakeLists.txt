cmake_minimum_required(VERSION 3.10)
project(WireProtocols LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)  # Optional, can also be set at configure time

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(GTest REQUIRED)
qt_standard_project_setup()

add_executable(client src/bin/client.cpp src/message/header.cpp)
add_executable(server src/bin/server.cpp src/message/header.cpp)
add_executable(tests test/test.cpp src/message/header.cpp)

set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(client PRIVATE Qt6::Core)
target_link_libraries(client PRIVATE Qt6::Gui)
target_link_libraries(client PRIVATE Qt6::Widgets)

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)