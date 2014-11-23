project(factory)

set(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(factory Base.cpp DerivedA.cpp main.cpp)

install(TARGETS factory RUNTIME DESTINATION bin)
