#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <journal.hpp>

#include <atomic>
#include <chrono>
#include <optional>
#include <string>

namespace application {

constexpr char APP_TAG[] = "App";

namespace graphics {

    constexpr int DEFAULT_WINDOW_WIDTH = 1920;
    constexpr int DEFAULT_WINDOW_HEIGHT = 1080;
    constexpr int DEFAULT_WINDOW_FSAA = 0;
    constexpr bool DEFAULT_WINDOW_VSYNC = false;
    constexpr bool DEFAULT_WINDOW_FULLSCREEN = false;
    constexpr bool DEFAULT_DEBUG = true;
    constexpr int DEFAULT_GL_CONTEXT_VERSION_MAJOR = 4;
    constexpr int DEFAULT_GL_CONTEXT_VERSION_MINOR = 6;
    constexpr std::string_view DEFAULT_WINDOW_TITLE = "Graphics application window";

    struct GLContextVersion {
        int major = 0;
        int minor = 0;
    };

    struct CreateWindowInfo {
        int32_t width = DEFAULT_WINDOW_WIDTH;
        int32_t height = DEFAULT_WINDOW_HEIGHT;
        int32_t fsaa = DEFAULT_WINDOW_FSAA;
        bool vsync = DEFAULT_WINDOW_VSYNC;
        bool fullscreen = DEFAULT_WINDOW_FULLSCREEN;
        bool debug = DEFAULT_DEBUG;
        bool centered = true;
        std::string_view title;
        GLContextVersion context_version = { DEFAULT_GL_CONTEXT_VERSION_MAJOR, DEFAULT_GL_CONTEXT_VERSION_MINOR };
    };

    struct Window {
        GLFWwindow* id = nullptr;
        int width = 0;
        int height = 0;
        int fsaa = 0;
    };

    namespace detail {

        inline void center_window( GLFWwindow* window, GLFWmonitor* monitor ) {
            if ( !monitor )
                return;

            const GLFWvidmode* mode = glfwGetVideoMode( monitor );
            if ( !mode )
                return;

            int monitor_x = 0, monitor_y = 0;
            glfwGetMonitorPos( monitor, &monitor_x, &monitor_y );

            int width = 0, height = 0;
            glfwGetWindowSize( window, &width, &height );

            glfwSetWindowPos(
                window, monitor_x + ( mode->width - width ) / 2, monitor_y + ( mode->height - height ) / 2 );
        }

        static inline void APIENTRY debug_output( GLenum source, GLenum type, unsigned int id, GLenum severity,
            [[maybe_unused]] GLsizei length, const char* message, [[maybe_unused]] const void* userParam ) {
            if ( id == 131169 || id == 131185 || id == 131218 || id == 131204 )
                return; // ignore these non-significant error codes

            std::string_view source_str;

            switch ( source ) {
            case GL_DEBUG_SOURCE_API:
                source_str = "Source: API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                source_str = "Source: Window System";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                source_str = "Source: Shader Compiler";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                source_str = "Source: Third Party";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                source_str = "Source: Application";
                break;
            case GL_DEBUG_SOURCE_OTHER:
                source_str = "Source: Other";
                break;
            }

            std::string_view type_str;

            switch ( type ) {
            case GL_DEBUG_TYPE_ERROR:
                type_str = "Type: Error";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                type_str = "Type: Deprecated Behaviour";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                type_str = "Type: Undefined Behaviour";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                type_str = "Type: Portability";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                type_str = "Type: Performance";
                break;
            case GL_DEBUG_TYPE_MARKER:
                type_str = "Type: Marker";
                break;
            case GL_DEBUG_TYPE_PUSH_GROUP:
                type_str = "Type: Push Group";
                break;
            case GL_DEBUG_TYPE_POP_GROUP:
                type_str = "Type: Pop Group";
                break;
            case GL_DEBUG_TYPE_OTHER:
                type_str = "Type: Other";
                break;
            }

            std::string severity_str;

            switch ( severity ) {
            case GL_DEBUG_SEVERITY_HIGH:
                severity_str = "Severity: high";
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                severity_str = "Severity: medium";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                severity_str = "Severity: low";
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                severity_str = "Severity: notification";
                break;
            }

            journal::verbose( "GL", "{} {} {} {} {}", source_str, type_str, severity_str, id, message );
        }

    } // namespace detail

} // namespace graphics

inline auto create_window( const graphics::CreateWindowInfo& info ) -> std::optional<graphics::Window> {
    using namespace graphics;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, info.context_version.major );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, info.context_version.minor );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, info.debug );
    glfwWindowHint( GLFW_VISIBLE, false );

    auto width = info.width;
    auto height = info.height;

    if ( info.fullscreen ) {
        auto primary = glfwGetPrimaryMonitor( );
        if ( primary ) {
            const auto mode = glfwGetVideoMode( primary );
            width = mode->width;
            height = mode->height;
        }
    }

    auto window = glfwCreateWindow(
        width, height, std::data( info.title ), info.fullscreen ? glfwGetPrimaryMonitor( ) : nullptr, nullptr );
    if ( !window ) {
        journal::error( APP_TAG, "Failed to create window" );
        return { };
    }

    glfwMakeContextCurrent( window );

    if ( info.vsync ) {
        glfwSwapInterval( 1 );
    }

    if ( !gladLoadGLLoader( reinterpret_cast<GLADloadproc>( glfwGetProcAddress ) ) ) {
        journal::error( APP_TAG, "Failed to initialize GLAD" );
        return { };
    }

    if ( info.centered ) {
        detail::center_window( window, glfwGetPrimaryMonitor( ) );
        glfwSetCursorPos( window, width / 2.f, height / 2.f );
    }

    if ( info.debug ) {
        int flags = 0;
        glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
        if ( flags & GL_CONTEXT_FLAG_DEBUG_BIT ) {
            glEnable( GL_DEBUG_OUTPUT );
            glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
            glDebugMessageCallback( detail::debug_output, nullptr );
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
        }

        journal::warning( APP_TAG, "OpenGL running in debugging mode" );
    }

    glfwShowWindow( window );

    Window w;
    w.id = window;
    w.fsaa = info.fsaa;

    glfwGetFramebufferSize( window, &w.width, &w.height );

    return { w };
}

inline auto destroy_window( graphics::Window& window ) {
    glfwDestroyWindow( window.id );
    window.id = nullptr;
}

inline auto process_window( graphics::Window& window ) {
    glfwSwapBuffers( window.id );
    glfwPollEvents( );
}

inline auto is_window_closed( graphics::Window& window ) noexcept -> bool {
    return glfwWindowShouldClose( window.id );
}

namespace utility {

    class Mainloop {
    public:
        Mainloop( ) = default;
        ~Mainloop( ) = default;
        Mainloop( const Mainloop& ) = delete;
        auto operator=( const Mainloop& ) -> Mainloop& = delete;

        template <typename EventType, typename... EventParamsType> auto post( EventParamsType&&... params ) -> void {
        }

        template <typename OnStep, typename OnFrame>
        auto run( const std::chrono::milliseconds& timestep, OnStep on_step, OnFrame on_frame ) {
            using namespace std::chrono_literals;

            auto current_time = std::chrono::high_resolution_clock::now( );
            auto last_time = current_time;
            auto accumulator = std::chrono::nanoseconds { 0 };
            auto timesteps = 0ull;

            while ( is_running( ) ) {
                last_time = current_time;
                current_time = std::chrono::high_resolution_clock::now( );
                const auto frame_time = current_time - last_time;
                accumulator
                    += std::clamp( frame_time, std::chrono::nanoseconds { 0 }, std::chrono::nanoseconds { 300ms } );

                while ( accumulator >= timestep ) {
                    accumulator -= timestep;

                    on_step( );

                    timesteps++;
                }

                on_frame( );
            }
        }

        auto is_running( ) noexcept -> bool {
            return !_done;
        }

        auto stop( ) -> void {
            _done = true;
        }

    private:
        std::atomic_bool _done = false;
    };

} // namespace utility

using Mainloop = utility::Mainloop;
using Window = graphics::Window;
using CreateWindowInfo = graphics::CreateWindowInfo;

} // namespace application

namespace utility {

template <typename T, size_t N> auto make_vector( const T ( &arr )[N] ) {
    std::vector<T> v;
    v.assign( arr, arr + N );
    return v;
}

template <typename T, size_t N> auto make_buffer( const T ( &arr )[N] ) {
    std::vector<uint8_t> v;
    v.resize( std::size( arr ) * sizeof( T ) );
    memcpy( &v[0], &arr[0], std::size( arr ) * sizeof( T ) );
    return v;
}

} // namespace utility