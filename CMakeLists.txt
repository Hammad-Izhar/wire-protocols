cmake_minimum_required(VERSION 3.10)
project(WireProtocols LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)  # Optional, can also be set at configure time

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(GTest REQUIRED)
qt_standard_project_setup()

file(GLOB_RECURSE SOURCE_FILES src/**/*.cpp)
file(GLOB_RECURSE TEST_SOURCE_FILES test/**/*.cpp)

foreach (FILE ${SOURCE_FILES})
    if (${FILE} MATCHES "src/bin/.*")
        list(REMOVE_ITEM SOURCE_FILES ${FILE})
    endif()
endforeach()

add_executable(client src/bin/client.cpp ${SOURCE_FILES})
add_executable(server src/bin/server.cpp ${SOURCE_FILES})
add_executable(test ${TEST_SOURCE_FILES} ${SOURCE_FILES})  

set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(client PRIVATE Qt6::Core)
target_link_libraries(client PRIVATE Qt6::Gui)
target_link_libraries(client PRIVATE Qt6::Widgets)

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test PRIVATE GTest::GTest GTest::Main)