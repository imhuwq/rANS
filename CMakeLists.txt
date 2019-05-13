project(rANS)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)

add_executable(uANS Source/uANS.cpp)
add_executable(encode Source/encode.cpp Source/Symbol.cpp Source/Helper.cpp)
add_executable(decode Source/decode.cpp Source/rANSDecoder.cpp Source/Symbol.cpp)
add_executable(test_ Source/test.cpp)
add_executable(draco Source/draco.cpp)
