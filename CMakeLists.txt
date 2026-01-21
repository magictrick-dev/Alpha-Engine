CMAKE_MINIMUM_REQUIRED(VERSION 3.29)

PROJECT(alpha)

ADD_EXECUTABLE(alpha WIN32
    source/platform/entry.cpp
    vendor/glad/glad.c
    vendor/glad/glad_wgl.c
)

TARGET_INCLUDE_DIRECTORIES(alpha PUBLIC
    include/
    source/
    vendor/
)
