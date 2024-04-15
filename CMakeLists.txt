cmake_minimum_required(VERSION 3.12)
project(radio-sample VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

add_executable(sample-rx)
add_executable(sample-tx)

protobuf_generate(TARGET sample-rx PROTOS nfiniity_cube_radio.proto)
protobuf_generate(TARGET sample-tx PROTOS nfiniity_cube_radio.proto)

target_sources(sample-rx PRIVATE sample_rx.cpp)
target_sources(sample-tx PRIVATE sample_tx.cpp)

target_include_directories(sample-rx PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(sample-tx PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sample-rx PUBLIC protobuf::libprotobuf Boost::headers)
target_link_libraries(sample-tx PUBLIC protobuf::libprotobuf Boost::headers)

get_target_property(OUT sample-rx LINK_LIBRARIES)
message(STATUS ${OUT})

