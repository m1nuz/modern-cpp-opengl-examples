#include <cube.hpp>
#include <example.hpp>

constexpr char EXAMPLE_TITLE[] = "Example03";

constexpr char VERTEX_SHADER[] = "#version 450 core\n"
                                 "#define MAX_INSTANCES 6\n"

                                 "layout(location = 0) in vec3 position;"
                                 "layout(location = 1) in vec2 texcoord;"
                                 "layout(location = 2) in vec3 normal;"

                                 "uniform mat4 projection_view;"

                                 "layout (std140, binding = 0) uniform MatrixBlock {"
                                 "mat4 model[MAX_INSTANCES];"
                                 "};"

                                 "out gl_PerVertex {"
                                 "vec4 gl_Position;"
                                 "};"

                                 "out VS_out {"
                                 "vec3 normal;"
                                 "vec2 texcoord;"
                                 "flat uint index;"
                                 "} vs_out;"

                                 "void main () {"
                                 "  vs_out.texcoord = texcoord;"
                                 "  vs_out.normal = vec3(model[gl_InstanceID] * vec4(normal, 0));"
                                 "  vs_out.index = gl_InstanceID;"
                                 "  gl_Position = projection_view * model[gl_InstanceID] * vec4(position, "
                                 "1.0);"
                                 "}";

constexpr char FRAGMENT_SHADER[] = "#version 450 core\n"
                                   "#define MAX_MATERIALS 6\n"

                                   "uniform vec3 color;"
                                   "layout(binding = 0) uniform sampler2DArray tex;"

                                   "struct Material {"
                                   "vec4 color;"
                                   "float layer;"
                                   "};"

                                   "layout (std140, binding = 1) uniform MaterialBlock {"
                                   "Material materials[MAX_MATERIALS];"
                                   "};"

                                   "in VS_out {"
                                   "vec3 normal;"
                                   "vec2 texcoord;"
                                   "flat uint index;"
                                   "} fs_in;"

                                   "layout (location = 0, index = 0) out vec4 frag_color;"
                                   "void main () {"
                                   "  vec3 n = normalize(fs_in.normal);"
                                   "  frag_color = materials[fs_in.index].color * texture(tex, "
                                   "vec3(fs_in.texcoord, materials[fs_in.index].layer));"
                                   "}";

struct Material {
    vec4 color;
    float layer;
    float padding[3];
};

const std::array<Material, 6> materials { Material { { 1.f, 0.f, 0.f, 1.f }, 0.f, {} },
    Material { { 0.f, 1.f, 0.f, 1.f }, 1.f, {} }, Material { { 0.f, 0.f, 1.f, 1.f }, 2.f, {} },
    Material { { 1.f, 1.f, 0.f, 1.f }, 3.f, {} }, Material { { 1.f, 0.f, 1.f, 1.f }, 4.f, {} },
    Material { { 0.f, 1.f, 1.f, 1.f }, 5.f, {} } };

int main( [[maybe_unused]] int argc, [[maybe_unused]] char* argv[] ) {
    gfx::Geometry geomerty;
    gfx::Texture texture;
    gfx::Buffer matrix_buffer;
    gfx::Buffer material_buffer;
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

                std::array<std::string_view, 6> names = { "../textures/brick_guiGen_512_d.tga",
                    "../textures/FloorBrick_JFCartoonyFloorBrickDirty_512_d.tga",
                    "../textures/Ground_MossyDirt_512_d.tga", "../textures/Metal_SciFiDiamondPlate_512_d.tga",
                    "../textures/Misc_OakbarrelOld_512_d.tga", "../textures/rock_guiWallSmooth09_512_d.tga" };
                uint32_t image_width = 0, image_height = 0;
                gfx::PixelFormat image_format = gfx::PixelFormat::unknown;
                std::vector<gfx::u8_buffer> pix_images;
                for ( auto n : names ) {
                    std::ifstream fs( std::data( n ), std::ios::in | std::ios::binary );
                    auto image = gfx::load_targa( fs );
                    if ( image ) {
                        image_width = image.value( ).width;
                        image_height = image.value( ).height;
                        image_format = image.value( ).format;
                        pix_images.push_back( image.value( ).pixels );
                    } else {
                        journal::error( EXAMPLE_TITLE, "Failed to load image '{}'", n );
                    }
                }

                if ( !pix_images.empty( ) ) {
                    texture = gfx::create_texture_array( { .width = image_width,
                        .height = image_height,
                        .depth = static_cast<uint32_t>( pix_images.size( ) ),
                        .format = image_format,
                        .pixels = pix_images } );
                }

                matrix_buffer = gfx::create_buffer( { .size = sizeof( mat4 ) * 6 } );
                material_buffer = gfx::create_buffer( { .size = sizeof materials } );

                std::array<mat4, 6> models
                    = { mat4 { 1.f }, mat4 { 1.f }, mat4 { 1.f }, mat4 { 1.f }, mat4 { 1.f }, mat4 { 1.f } };

                gfx::update_buffer( material_buffer, materials );
                gfx::update_buffer( matrix_buffer, models );
            },
        .on_update =
            [&]( ) {
                using namespace glm;

                angle += 0.01f;

                std::array<mat4, 6> models;
                const vec3 angles[6] = {
                    { 0, 1, 1 },
                    { 1, 0, -1 },
                    { -1, -1, 0 },
                    { 1, 0, 1 },
                    { 0, 1, 1 },
                    { -1, 0, -1 },
                };

                for ( int i = 0; i < 6; i++ ) {
                    models[i] = translate( mat4( 1.f ), vec3( ( i % 2 ) * 4 - 1.5f, ( i % 3 ) * 3 - 3.f, 0 ) );
                    models[i] = rotate( models[i], angle * angles[i][0], vec3( 1, 0, 0 ) );
                    models[i] = rotate( models[i], angle * angles[i][1], vec3( 0, 1, 0 ) );
                    models[i] = rotate( models[i], angle * angles[i][2], vec3( 0, 0, 1 ) );
                    models[i] = scale( models[i], vec3( 1.f ) );
                }

                gfx::update_buffer( matrix_buffer, models );
            },
        .on_present =
            [&]( auto w, auto h ) {
                using namespace glm;

                const auto projection = perspective( 45.f, (float)w / (float)h, 1.f, 1000.f );
                auto view = translate( mat4( 1.f ), vec3( 0, 0, -10.f ) );
                const auto projection_view = projection * view;

                commands << gfx::clear_frame { { 0.4, 0.4, 0.4, 1 } };
                commands << gfx::bind_pipeline { pipeline };
                commands << gfx::bind_texture { 0, texture.id };
                commands << gfx::bind_buffer { gfx::BufferType::Uniform, matrix_buffer, pipeline, "MatrixBlock" };
                commands << gfx::bind_buffer { gfx::BufferType::Uniform, material_buffer, pipeline, "MaterialBlock" };
                commands << gfx::set_uniform { pipeline, "projection_view", projection_view };
                commands << gfx::draw_geometry { geomerty, 6 };

                gfx::present( { &commands } );
            },
        .on_cleanup =
            [&]( ) {
                gfx::destroy_geometry( geomerty );
                gfx::destroy_shader( vertex_shader );
                gfx::destroy_shader( fragment_shader );
                gfx::destroy_program_pipeline( pipeline );
                gfx::destroy_texture( texture );
                gfx::destroy_buffer( material_buffer );
                gfx::destroy_buffer( matrix_buffer );
            } } );
}