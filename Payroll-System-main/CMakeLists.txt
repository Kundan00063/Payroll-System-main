cmake_minimum_required(VERSION 3.14)
project(PayrollSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(asio crow)

set(BACKEND_SOURCES
    backend/Employee.cpp
    backend/FullTimeEmployee.cpp
    backend/Contractor.cpp
    backend/Intern.cpp
    backend/FileManager.cpp
    backend/Analytics.cpp
    backend/PayrollSystem.cpp
    backend/Server.cpp
)

add_executable(payroll_server ${BACKEND_SOURCES})

target_include_directories(payroll_server PRIVATE
    ${CMAKE_SOURCE_DIR}/backend
    ${asio_SOURCE_DIR}/asio/include
)

target_link_libraries(payroll_server PRIVATE Crow::Crow)

if(WIN32)
    target_link_libraries(payroll_server PRIVATE ws2_32 wsock32 mswsock)
endif()

if(MSVC)
    target_compile_options(payroll_server PRIVATE /W4 /utf-8)
else()
    target_compile_options(payroll_server PRIVATE -Wall -Wextra -pedantic)
endif()

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)

add_custom_command(TARGET payroll_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/frontend ${CMAKE_BINARY_DIR}/frontend
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
)

message(STATUS "Build the project with: cmake -B build && cmake --build build")
message(STATUS "Run from build directory: ./payroll_server (or payroll_server.exe on Windows)")
