add_library(Vector SHARED Vector.cpp)
target_include_directories(Vector PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Number
)
target_link_libraries(Vector Number)
