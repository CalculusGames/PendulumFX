cmake_minimum_required(VERSION 3.30)

set(PROJECT_VERSION "1.0.0")
project(PendulumFX
    LANGUAGES CXX
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Pendulum simulation with cmdfx"
    HOMEPAGE_URL "https://github.com/CalculusGames/PendulumFX"
)

set(CMAKE_CXX_STANDARD 17)

add_executable(PendulumFX main.cpp pendulum.hpp)
set_target_properties(PendulumFX PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Libraries
find_package(cmdfx REQUIRED)
message(STATUS "Found cmdfx: ${cmdfx_DIR}")
target_link_libraries(PendulumFX PRIVATE cmdfx::cmdfx)
if(UNIX)
	target_link_libraries(PendulumFX PRIVATE m)
endif()

# macOS Frameworks
if(APPLE)
    target_link_libraries(PendulumFX PRIVATE
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework Carbon"
    )
endif()
