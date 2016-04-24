cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(message_posting VERSION 1.0 LANGUAGES CXX C)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(message_server
  server/server_main.cc
  server/server.cc)

add_executable(message_client
  client/client_main.cc
  client/client.cc)

target_include_directories(message_server
  PRIVATE server utils)

target_include_directories(message_client
  PRIVATE client utils)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(message_server PRIVATE -std=c++11)
  target_compile_options(message_client PRIVATE -std=c++11)
endif()

target_link_libraries(message_server PRIVATE Threads::Threads)
target_link_libraries(message_client PRIVATE Threads::Threads)
