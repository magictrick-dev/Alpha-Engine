CMAKE_MINIMUM_REQUIRED(VERSION 3.28)

PROJECT(alpha)

IF (WIN32)

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

ENDIF(WIN32)

IF (UNIX)

    ADD_EXECUTABLE(alpha 
        source/platform/entry.cpp
        vendor/glad/glad.c
    )

    FIND_PACKAGE(X11 REQUIRED)
    TARGET_LINK_LIBRARIES(alpha PUBLIC ${X11_LIBRARIES})

    TARGET_INCLUDE_DIRECTORIES(alpha PUBLIC
        include/
        source/
        vendor/
    )

ENDIF(UNIX)
