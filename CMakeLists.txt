cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(printer-enclosure-monitor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)

if(MSVC)
  set (Boost_USE_STATIC_RUNTIME ON)
  add_definitions(-D_WIN32_WINNT=0x0601)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
  add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
  add_compile_options(/MP)
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(CURL REQUIRED) 

file(GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)


file(GLOB APP_FILES
  beast.hpp
  http_session.cpp
  http_session.hpp
  listener.cpp
  listener.hpp
  main.cpp
  net.hpp
  shared_state.cpp
  shared_state.hpp
  websocket_session.cpp
  websocket_session.hpp
  serial.cpp
  serial.hpp
  index.html
)
#   beast.hpp
#   http_session.cpp
#   http_session.hpp
#   listener.cpp
#   listener.hpp
#   main.cpp
#   net.hpp
#   shared_state.cpp
#   shared_state.hpp
#   websocket_session.cpp
#   websocket_session.hpp
#   chat_client.html
#   README.md)

add_executable(printer-enclosure-monitor ${APP_FILES} ${BEAST_FILES})

if(NOT WIN32)
  find_package(Threads)
  target_link_libraries(printer-enclosure-monitor PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY} ${CURL_LIBRARIES})
endif()

add_custom_target(copy-files ALL
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_SOURCE_DIR}/index.html
  ${CMAKE_CURRENT_BINARY_DIR}/index.html
)

# add_custom_target(copy-test-makefile ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/index.html)
# add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index.html
#                    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/index.html
#                                                     ${CMAKE_CURRENT_BINARY_DIR}/index.html
#                    DEPENDS ${CMAKE_SOURCE_DIR}/index.html)