cmake_minimum_required(VERSION 3.10)
project(ProcessKillerLinux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(killer Killer.cpp)

add_executable(user User.cpp)

install(TARGETS killer user
        RUNTIME DESTINATION bin)