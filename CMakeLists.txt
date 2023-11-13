cmake_minimum_required(VERSION 3.5)
project(P2P_encrypted_chat)

set(LANGUAGE CXX)
set(CMAKE_BUILD_TYPE Debug)

set(SRCS
    main.cpp
)

set(HEADERS
)

add_executable(SecChat ${SRCS} ${HEADERS})
