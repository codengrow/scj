project(pipeline)

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")

add_executable(pipeline pipeline_1.h pipeline_2.h main.cpp)

install(TARGETS pipeline RUNTIME DESTINATION bin)
