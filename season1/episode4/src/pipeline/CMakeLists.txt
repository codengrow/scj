project(pipeline)

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")

add_executable(pipeline main.cpp)

install(TARGETS pipeline RUNTIME DESTINATION bin)
