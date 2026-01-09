# Файлы статической библиотеки лежат в папке Number, динамической - в папке Vector, исполняемый файл - в папке Test.
cmake_minimum_required(VERSION 3.10)
project(LibsProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Number)
add_subdirectory(Vector)
add_subdirectory(Test)
