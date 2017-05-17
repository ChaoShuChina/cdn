cmake_minimum_required(VERSION 2.8.4)
project(cdn)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${PROJECT_SOURCE_DIR}/lib)

set(SOURCE_FILES
        lib/lib_io.h
        lib/lib_time.h
        cdn.cpp
        cdn.h
        deploy.cpp
        deploy.h
        getPaths.cpp
        importOpt.cpp
        io.cpp
        MCFSimplex.cpp
        MCFSimplex.h
        readGraph.cpp
        Tree.cpp
        Tree.h
        include.h
        Node.h)

add_executable(cdn ${SOURCE_FILES})