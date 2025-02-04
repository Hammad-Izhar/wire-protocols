cmake_minimum_required(VERSION 3.10)
project(WireProtocols LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

add_executable(client src/main.cpp)

set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(client PRIVATE Qt6::Core)
target_link_libraries(client PRIVATE Qt6::Gui)
target_link_libraries(client PRIVATE Qt6::Widgets)

install(TARGETS client
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)