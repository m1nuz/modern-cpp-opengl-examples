#include <cube.hpp>
#include <example.hpp>
#include <quad.hpp>

constexpr char EXAMPLE_TITLE[] = "Example04";

constexpr char VERTEX_SHADER[] = R"(
#version 450 core

uniform mat4 mvp;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

out gl_PerVertex {
    vec4 gl_Position;
};

void main () {
    gl_Position = mvp * vec4(position, 1.0);
}
)";

constexpr char FRAGMENT_SHADER[] = R"(
#version 450 core

uniform vec3 color;

layout (location = 0, index = 0) out vec4 frag_color;

void main () {
    frag_color = vec4(color, 1.0);
}
)";

constexpr char POST_VERTEX_SHADER[] = R"(
#version 450 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texcoord;

out VS_out {
    vec2 texcoord;
}
vs_out;

out gl_PerVertex {
    vec4 gl_Position;
};

void main( ) {
    vs_out.texcoord = texcoord;
    gl_Position = vec4( position, 1.0 );
}
)";

constexpr char POST_FRAGMENT_SHADER[] = R"(
#version 450 core

layout( binding = 0 ) uniform sampler2D color_map;

in VS_out {
    vec2 texcoord;
}
fs_in;

out vec4 fragcolor;

void main( ) {
    fragcolor = texture( color_map, fs_in.texcoord );
}
)";

int main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] ) {
    gfx::Geometry geomerty, screenquad;
    gfx::Shader vertex_shader, post_vertex_shader;
    gfx::Shader fragment_shader, post_fragment_shader;
    gfx::ProgramPipeline pipeline, post_pipeline;
    gfx::CommandQueue commands, post_commands;
    gfx::Framebuffer sample_framebuffer;
    gfx::Framebuffer simple_framebuffer;
    gfx::Renderbuffer renderbuffer;
    gfx::Renderbuffer depthbuffer;
    gfx::Texture colortexture;
    gfx::Texture depthtexture;

    float angle = 0.f;

    ExampleApp example;
    return example.run( { .title = EXAMPLE_TITLE,
        .on_init =
            [&]( ) {
                journal::info( EXAMPLE_TITLE, "Running..." );

                commands.depth_stencil.depth_test = true;
                commands.depth_stencil.depth_write = true;
                post_commands.depth_stencil.depth_test = false;
                post_commands.depth_stencil.depth_write = true;

                geomerty = gfx::create_geometry( { .vertices_num = CUBE_NUM_VERTICES,
                    .indices_num = CUBE_NUM_INDICES,
                    .format = gfx::VertexFormat::v3t2n3_f32ui16,
                    .vertices = utility::make_buffer( cube_vertices ),
                    .indices = utility::make_vector( cube_indices ) } );

                screenquad = gfx::create_geometry( { .vertices_num = QUAD_NUM_VERTICES,
                    .indices_num = QUAD_NUM_INDICES,
                    .format = gfx::VertexFormat::v3t2_f32ui16,
                    .vertices = utility::make_buffer( quad_vertices ),
                    .indices = utility::make_vector( quad_indices ) } );

                vertex_shader = gfx::create_shader( { .type = gfx::ShaderType::vertex, .source = VERTEX_SHADER } );

                post_vertex_shader
                    = gfx::create_shader( { .type = gfx::ShaderType::vertex, .source = POST_VERTEX_SHADER } );

                fragment_shader = gfx::create_shader( { .type = gfx::ShaderType::fragmet, .source = FRAGMENT_SHADER } );

                post_fragment_shader
                    = gfx::create_shader( { .type = gfx::ShaderType::fragmet, .source = POST_FRAGMENT_SHADER } );

                pipeline = gfx::create_program_pipeline( { .shaders = { vertex_shader, fragment_shader } } );

                post_pipeline
                    = gfx::create_program_pipeline( { .shaders = { post_vertex_shader, post_fragment_shader } } );

                const auto samples = 16;

                renderbuffer = gfx::create_renderbuffer( { .format = gfx::PixelFormat::rgb16f,
                    .width = gfx::default_framebuffer.width,
                    .height = gfx::default_framebuffer.height,
                    .samples = samples } );

                depthbuffer = gfx::create_renderbuffer( { .format = gfx::PixelFormat::depth,
                    .width = gfx::default_framebuffer.width,
                    .height = gfx::default_framebuffer.height,
                    .samples = samples } );

                colortexture = gfx::create_texture( { .width = gfx::default_framebuffer.width,
                    .height = gfx::default_framebuffer.height,
                    .format = gfx::PixelFormat::rgb16f,
                    .mipmaps = false,
                    .filter = gfx::TextureFiltering::None } );

                depthtexture = gfx::create_texture( { .width = gfx::default_framebuffer.width,
                    .height = gfx::default_framebuffer.height,
                    .format = gfx::PixelFormat::depth,
                    .mipmaps = false,
                    .filter = gfx::TextureFiltering::None } );

                sample_framebuffer = gfx::create_framebuffer( { .width = gfx::default_framebuffer.width,
                    .height = gfx::default_framebuffer.height,
                    .attachments = { { GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer.id },
                        { GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer.id } } } );

                if ( !sample_framebuffer ) {
                    journal::error( EXAMPLE_TITLE, "Framebuffer {} invalid or incomplete {:x}", sample_framebuffer.id,
                        sample_framebuffer.status );
                }

                simple_framebuffer = gfx::create_framebuffer( { .width = gfx::default_framebuffer.width,
                    .height = gfx::default_framebuffer.height,
                    .attachments = { { GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colortexture.id },
                        { GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthtexture.id } } } );

                if ( !simple_framebuffer ) {
                    journal::error( EXAMPLE_TITLE, "Framebuffer {} invalid or incomplete {:x}", simple_framebuffer.id,
                        simple_framebuffer.status );
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

                commands << gfx::bind_framebuffer { sample_framebuffer };
                commands << gfx::clear_framebuffer { sample_framebuffer, { 0.4, 0.4, 0.4, 1 } };
                commands << gfx::bind_pipeline { pipeline };
                commands << gfx::set_uniform { pipeline, "mvp", mvp };
                commands << gfx::set_uniform { pipeline, "color", vec3 { 1.0f, 1.0f, 0.0f } };
                commands << gfx::draw_geometry { geomerty };

                post_commands << gfx::blit_framebuffer { sample_framebuffer, simple_framebuffer };
                post_commands << gfx::bind_framebuffer { gfx::default_framebuffer };
                post_commands << gfx::clear_framebuffer { { 0.0, 0.0, 0.0, 1 } };
                post_commands << gfx::bind_pipeline { post_pipeline };
                post_commands << gfx::bind_texture { 0, colortexture.id };
                post_commands << gfx::draw_geometry { screenquad };

                gfx::present( { &commands, &post_commands } );
            },
        .on_cleanup =
            [&]( ) {
                gfx::destroy_geometry( geomerty );
                gfx::destroy_geometry( screenquad );
                gfx::destroy_shader( vertex_shader );
                gfx::destroy_shader( fragment_shader );
                gfx::destroy_shader( post_vertex_shader );
                gfx::destroy_shader( post_fragment_shader );
                gfx::destroy_program_pipeline( pipeline );
                gfx::destroy_program_pipeline( post_pipeline );
                gfx::destroy_framebuffer( sample_framebuffer );
                gfx::destroy_framebuffer( simple_framebuffer );
                gfx::destroy_renderbuffer( renderbuffer );
                gfx::destroy_renderbuffer( depthbuffer );
                gfx::destroy_texture( colortexture );
                gfx::destroy_texture( depthtexture );
            } } );
}