set(APP_NAME example01)

add_executable(${APP_NAME}
    example01.cpp
)

target_compile_options(${APP_NAME}
    PUBLIC
        -pthread
        -pedantic
        -Wall
        -Wextra
        #-Werror
)

target_compile_features(${APP_NAME}
    PUBLIC
        cxx_std_20
)

target_include_directories(${APP_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
)

target_link_libraries(${APP_NAME}
    PUBLIC
        fmt
        glfw
        glad
        stdc++
        stdc++fs
        Threads::Threads
)
