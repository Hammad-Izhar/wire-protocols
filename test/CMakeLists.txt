file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/**/*.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*bin/.*")

file(GLOB_RECURSE SERVER_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/bin/server/*.cpp)
list(FILTER SERVER_SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")

file(GLOB_RECURSE TEST_SOURCE_FILES ${CMAKE_SOURCE_DIR}/test/**/*.cpp)

file(GLOB_RECURSE SERVER_QT_HEADERS ${CMAKE_SOURCE_DIR}/include/server/model/client_handler.hpp ${CMAKE_SOURCE_DIR}/include/server/model/tcp_server.hpp${CMAKE_SOURCE_DIR}/include/models/message_handler.hpp ${CMAKE_SOURCE_DIR}/include/models/user.hpp)

qt_add_executable(test
    ${SOURCE_FILES}
    ${SERVER_SOURCE_FILES}
    ${TEST_SOURCE_FILES}
)

target_link_libraries(test PRIVATE GTest::GTest GTest::Main)
target_link_libraries(test PRIVATE Qt6::Core Qt6::Network OpenSSL::SSL OpenSSL::Crypto)