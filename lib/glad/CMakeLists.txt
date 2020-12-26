set(GLAD_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE PATH "GLAD include dir")

set(LIB_NAME glad)

set(SOURCES src/glad.c)

add_library(${LIB_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${LIB_NAME} 
    PUBLIC
        ${GLAD_INCLUDE_DIRS}
)

target_link_libraries(${LIB_NAME}
    PUBLIC
        ${CMAKE_DL_LIBS}
)
