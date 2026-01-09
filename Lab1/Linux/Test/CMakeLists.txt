add_executable(main main.cpp)
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Number
    ${CMAKE_CURRENT_SOURCE_DIR}/../Vector
)
target_link_libraries(main Vector)
