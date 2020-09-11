cmake_minimum_required(VERSION 3.10)
project (tiny_data_client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(${PROJECT_NAME}_headers
  src/data_processing.h
  src/data_sink.h
  src/data_source.h
  src/secret_channel.h
  src/types.h
)

set(${PROJECT_NAME}_sources
  src/data_processing.cpp
  src/data_sink.cpp
  src/data_source.cpp
  src/secret_channel.cpp
  src/main.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:pthread>
)
