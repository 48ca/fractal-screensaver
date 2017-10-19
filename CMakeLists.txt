project(fss)
cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Ofast")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Ofast")

find_package(SDL REQUIRED)
find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif (OPENMP_FOUND)

include_directories(${SDL_INCLUDE_DIR})
link_libraries(${SDL_LIBRARY})

add_executable(fss main.cpp)
