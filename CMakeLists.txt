cmake_minimum_required (VERSION 3.10)
project(test-asio)

#set(Boost_USE_MULTITHREAD ON)
#find_package(Boost REQUIRED asio)
find_package(Boost REQUIRED program_options system)
find_package(Threads)

add_executable(test-asio main.cpp)

target_link_libraries(test-asio ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(test-asio PRIVATE -DBOOST_ERROR_CODE_HEADER_ONLY)