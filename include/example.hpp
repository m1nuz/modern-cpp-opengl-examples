#pragma once

#include <application.hpp>
#include <graphics.hpp>

struct RunExampleAppInfo {
    std::string_view title;
    std::function<void( )> on_init = []( ) {};
    std::function<void( )> on_update = []( ) {};
    std::function<void( int, int )> on_present;
    std::function<void( )> on_cleanup = []( ) {};
};

class ExampleApp {
public:
    ExampleApp( ) = default;
    ExampleApp( const ExampleApp& ) = delete;
    auto operator=( const ExampleApp& ) -> ExampleApp& = delete;

    auto run( const RunExampleAppInfo& info ) -> int {
        return _run( info.title, info.on_init, info.on_update, info.on_present, info.on_cleanup );
    }

private:
    template <typename OnInit, typename OnUpdate, typename OnPresent, typename OnCleanup>
    auto _run( std::string_view title, OnInit on_init, OnUpdate on_update, OnPresent on_present, OnCleanup on_cleanup )
        -> int {
        using namespace std::literals;

        glfwSetErrorCallback(
            []( int error, const char* description ) { journal::error( _tag, "Error {} {}", error, description ); } );

        if ( !glfwInit( ) )
            return EXIT_FAILURE;
        atexit( glfwTerminate );

        auto window = application::create_window( { .width = 1440, .height = 1080, .title = title } );
        if ( !window ) {
            journal::error( _tag, "Couldn't create window" );
            return EXIT_FAILURE;
        }

        graphics::default_framebuffer.width = window->width;
        graphics::default_framebuffer.height = window->height;

        on_init( );

        _mainloop.run( std::chrono::milliseconds { 16ms }, on_update, [&]( ) {
            if ( application::is_window_closed( *window ) ) {
                _mainloop.stop( );
            }

            on_present( window->width, window->height );

            application::process_window( *window );
        } );

        on_cleanup( );

        return EXIT_SUCCESS;
    }

    static constexpr std::string_view _tag = "Example";
    application::Window _window;
    application::Mainloop _mainloop;
};