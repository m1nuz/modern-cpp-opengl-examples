#include <cube.hpp>
#include <example.hpp>

constexpr char EXAMPLE_TITLE[] = "Example02";

constexpr char VERTEX_SHADER[] = "#version 450 core\n"
                                 "uniform mat4 mvp;"

                                 "layout(location = 0) in vec3 position;"
                                 "layout(location = 1) in vec2 texcoord;"
                                 "layout(location = 2) in vec3 normal;"

                                 "out gl_PerVertex {"
                                 "vec4 gl_Position;"
                                 "};"

                                 "out VS_out {"
                                 "vec2 texcoord;"
                                 "} vs_out;"

                                 "void main () {"
                                 "  vs_out.texcoord = texcoord;"
                                 "  gl_Position = mvp * vec4(position, 1.0);"
                                 "}";

constexpr char FRAGMENT_SHADER[] = "#version 450 core\n"
                                   "uniform vec3 color;"
                                   "layout(binding = 0) uniform sampler2D tex;"

                                   "in VS_out {"
                                   "vec2 texcoord;"
                                   "} fs_in;"

                                   "layout (location = 0, index = 0) out vec4 frag_color;"
                                   "void main () {"
                                   "  frag_color = vec4(color, 1.0) * vec4(texture(tex, fs_in.texcoord).xyz, "
                                   "1.0);"
                                   "}";

int main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] ) {
    gfx::Geometry geomerty;
    gfx::Texture texture;
    gfx::Shader vertex_shader;
    gfx::Shader fragment_shader;
    gfx::ProgramPipeline pipeline;
    gfx::CommandQueue commands;

    float angle = 0.f;

    ExampleApp example;
    return example.run( { .title = EXAMPLE_TITLE,
        .on_init =
            [&]( ) {
                journal::info( EXAMPLE_TITLE, "Running..." );

                commands.depth_stencil.depth_test = true;
                commands.depth_stencil.depth_write = true;

                geomerty = gfx::create_geometry( { .vertices_num = CUBE_NUM_VERTICES,
                    .indices_num = CUBE_NUM_INDICES,
                    .format = gfx::VertexFormat::v3t2n3_f32ui16,
                    .vertices = utility::make_buffer( cube_vertices ),
                    .indices = utility::make_vector( cube_indices ) } );

                vertex_shader = gfx::create_shader( { .type = gfx::ShaderType::vertex, .source = VERTEX_SHADER } );

                fragment_shader = gfx::create_shader( { .type = gfx::ShaderType::fragmet, .source = FRAGMENT_SHADER } );

                pipeline = gfx::create_program_pipeline( { .shaders = { vertex_shader, fragment_shader } } );

                std::ifstream fs( "../textures/texture.tga", std::ios::in | std::ios::binary );
                auto image = gfx::load_targa( fs );
                if ( image ) {
                    texture = gfx::create_texture( { .width = image->width,
                        .height = image->height,
                        .format = image->format,
                        .pixels = image->pixels } );
                } else {
                    journal::error( EXAMPLE_TITLE, "Failed to load image" );
                }
            },
        .on_update = [&]( ) { angle += 0.01f; },
        .on_present =
            [&]( auto w, auto h ) {
                using namespace glm;

                auto model = translate( mat4( 1.f ), vec3( 0, 0, 0 ) );
                model = rotate( model, angle, vec3( 1, 0, 0 ) );
                model = rotate( model, 0.f, vec3( 0, 1, 0 ) );
                model = rotate( model, angle, vec3( 0, 0, 1 ) );
                model = scale( model, vec3( 1.f ) );

                const auto projection = perspective( 45.f, (float)w / (float)h, 1.f, 1000.f );
                auto view = translate( mat4( 1.f ), vec3( 0, 0, -5.f ) );
                const auto mvp = projection * view * model;

                commands << gfx::clear_framebuffer { { 0.4, 0.4, 0.4, 1 } };
                commands << gfx::bind_pipeline { pipeline };
                commands << gfx::bind_texture { 0, texture.id };
                commands << gfx::set_uniform { pipeline, "mvp", mvp };
                commands << gfx::set_uniform { pipeline, "color", vec3 { 1.0f, 1.0f, 1.0f } };
                commands << gfx::draw_geometry { geomerty };

                gfx::present( { &commands } );
            },
        .on_cleanup =
            [&]( ) {
                gfx::destroy_geometry( geomerty );
                gfx::destroy_shader( vertex_shader );
                gfx::destroy_shader( fragment_shader );
                gfx::destroy_program_pipeline( pipeline );
                gfx::destroy_texture( texture );
            } } );
}