include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.2
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_INSTALL=OFF
  UPDATE_DISCONNECTED ON
)

FetchContent_MakeAvailable(glfw)

if(IS_DIRECTORY "${glfw_SOURCE_DIR}")
    set_property(DIRECTORY ${glfw_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()
