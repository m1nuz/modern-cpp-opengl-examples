#include <example.hpp>

constexpr char EXAMPLE_TITLE[] = "Example01";

constexpr gfx::v3_t points[] = {
    // vertices of triangle
    vec3 { 0.0f, 0.5f, 0.0f }, // 1
    vec3 { 0.5f, -0.5f, 0.0f }, // 2
    vec3 { -0.5f, -0.5f, 0.0f } // 3
};

int main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] ) {

    gfx::Geometry geomerty;
    gfx::CommandQueue commands;

    ExampleApp example;
    return example.run( { .title = EXAMPLE_TITLE,
        .on_init =
            [&]( ) {
                journal::info( EXAMPLE_TITLE, "Running..." );

                geomerty = gfx::create_geometry( { .vertices_num = 3,
                    .format = gfx::VertexFormat::v3_f32,
                    .vertices = utility::make_buffer( points ) } );
            },
        .on_present =
            [&]( auto, auto ) {
                commands << gfx::clear_frame { vec4 { 0.4, 0.4, 0.4, 1 } };
                commands << gfx::bind_vao { geomerty };
                commands << gfx::draw_elements { .num_elements = 3 };

                gfx::present( { &commands } );
            },
        .on_cleanup = [&]( ) { gfx::destroy_geometry( geomerty ); } } );
}