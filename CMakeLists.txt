cmake_minimum_required(VERSION 3.16)
project(WireProtocols LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)  # Optional, can also be set at configure time
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find necessary Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)

# Include project directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES src/**/*.cpp)
file(GLOB_RECURSE CLIENT_SOURCE_FILES src/bin/client/*.cpp)
list(FILTER CLIENT_SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")
file(GLOB_RECURSE SERVER_SOURCE_FILES src/bin/server/*.cpp)
list(FILTER SERVER_SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")
file(GLOB_RECURSE TEST_SOURCE_FILES test/*.cpp src/bin/server/db/*.cpp)
list(FILTER TEST_SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")
file(GLOB_RECURSE CLIENT_QT_HEADERS include/client/gui/*.hpp include/client/gui/*.h include/client/model/tcp_client.hpp include/client/model/session.hpp include/models/message_handler.hpp include/models/user.hpp)
file(GLOB_RECURSE SERVER_QT_HEADERS include/server/model/client_handler.hpp include/server/model/tcp_server.hpp include/models/message_handler.hpp include/models/user.hpp)

foreach (FILE ${SOURCE_FILES})
    if (FILE MATCHES "src/bin/.*")
        list(REMOVE_ITEM SOURCE_FILES ${FILE})
    endif()
endforeach()

# Remove main files from their respective sources
list(REMOVE_ITEM CLIENT_SOURCE_FILES src/bin/client/main.cpp)
list(REMOVE_ITEM SERVER_SOURCE_FILES src/bin/server/main.cpp)

# Define Client executable
qt_add_executable(client
    src/bin/client/main.cpp
    ${CLIENT_SOURCE_FILES}
    ${SOURCE_FILES}
    ${CLIENT_QT_HEADERS}
)

qt_add_resources(client_resources "resources.qrc")

target_sources(client PRIVATE ${client_resources})

set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    AUTOMOC ON  # Ensures Q_OBJECT macro is processed
    AUTOUIC ON  # Enable Qt UI file processing (if needed)
    AUTORCC ON  # Enable Qt resource processing (if needed)
)

target_link_libraries(client PRIVATE Qt6::Widgets Qt6::Network)

# Define Server executable
qt_add_executable(server
    src/bin/server/main.cpp
    ${SERVER_SOURCE_FILES}
    ${SOURCE_FILES}
    ${SERVER_QT_HEADERS}
)
set_target_properties(server PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    AUTOMOC ON  # Ensures Q_OBJECT macro is processed
    AUTOUIC ON  # Enable Qt UI file processing (if needed)
    AUTORCC ON  # Enable Qt resource processing (if needed)
)

target_link_libraries(server PRIVATE Qt6::Core Qt6::Network)
target_link_libraries(server PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Define Test executable
qt_add_executable(test
    ${SOURCE_FILES}
    ${SERVER_SOURCE_FILES}
    ${TEST_SOURCE_FILES}
    ${SERVER_QT_HEADERS}
)

set_target_properties(test PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    AUTOMOC ON  # Ensures Q_OBJECT macro is processed
    AUTOUIC ON  # Enable Qt UI file processing (if needed)
    AUTORCC ON  # Enable Qt resource processing (if needed)
)


target_link_libraries(test PRIVATE GTest::GTest GTest::Main)
target_link_libraries(test PRIVATE Qt6::Core Qt6::Network)
target_link_libraries(test PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Print included sources for debugging
message(STATUS "Shared library source files:")
foreach(FILE ${SOURCE_FILES})
    message(STATUS " - ${FILE}")
endforeach()

message(STATUS "Client source files:")
foreach(FILE ${CLIENT_SOURCE_FILES})
    message(STATUS " - ${FILE}")
endforeach()

message(STATUS "Server source files:")
foreach(FILE ${SERVER_SOURCE_FILES})
    message(STATUS " - ${FILE}")
endforeach()

message(STATUS "Test source files:")
foreach(FILE ${TEST_SOURCE_FILES})
    message(STATUS " - ${FILE}")
endforeach()
