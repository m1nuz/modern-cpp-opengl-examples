// The MIT License (MIT)
//
// Copyright (c) 2021 Michael Poddubnyi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <glad/glad.h>

#include <glmath.hpp>

#include <string>
#include <variant>
#include <vector>

// For loading images
#include <fstream>

namespace graphics {

constexpr char GRAPHICS_TAG[] = "GL";

enum class PixelFormat : std::uint32_t {
    unknown,
    r8,
    rg8,
    rgb8,
    rgba8,
    bgr8,
    bgra8,
    r16f,
    r32f,
    rgb16f,
    rgba16f,
    rgb32f,
    rgba32f,
    depth
};

enum class VertexFormat : std::uint32_t {
    unknown,
    v3_f32,
    v3_f32ui16,
    v3n3_f32ui16,
    v3t2_f32ui16,
    v3t2n3_f32ui16,
    v3uv2n3t3_f32ui16
};

///
/// Vertex formats
///
struct v3t2n3_t {
    vec3 position;
    vec2 uv;
    vec3 normal;
};
struct v3_t {
    vec3 position;
};
struct v3n3_t {
    vec3 position;
    vec3 normal;
};
struct v3t2_t {
    vec3 position;
    vec2 uv;
};
struct v3uv2n3t3_t {
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
};

///
/// Pipeline states
///
struct ColorBlendState {
    bool enable = false;
    uint32_t sfactor = GL_ONE;
    uint32_t dfactor = GL_ONE;
};

struct RasterizerState {
    bool cull_faces = false;
    uint32_t cull_mode = GL_BACK;
};

struct DepthStencilState {
    bool depth_test = false;
    bool depth_write = true;
    uint32_t depth_func = GL_LESS;
};

///
/// Graphics objects
///
struct Texture {
    auto is_valid( ) const noexcept -> bool {
        return id != 0;
    }

    uint32_t id = 0;
    uint32_t target = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
};

enum class TextureFiltering {
    None,
    Bilinear,
    Trilinear,
    Anisotropic,
};

using u8_buffer = std::vector<uint8_t>;

struct CreateTextureInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    PixelFormat format = PixelFormat::unknown;
    bool mipmaps = true;
    uint32_t levels = 4;
    TextureFiltering filter = TextureFiltering::Trilinear;
    u8_buffer pixels;
};

struct CreateTextureArrayInfo {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    PixelFormat format = PixelFormat::unknown;
    bool mipmaps = true;
    uint32_t levels = 4;
    TextureFiltering filter = TextureFiltering::Trilinear;
    std::vector<u8_buffer> pixels;
};

using CreateTextureCubemapInfo = CreateTextureArrayInfo;

struct TextureSampler {
    auto is_valid( ) const noexcept -> bool {
        return id != 0;
    }

    uint32_t id = 0;
    uint32_t target = 0;
};

enum class BufferType { Unknown, VertexArray, VertexElements, Uniform };

struct Buffer {
    auto is_valid( ) const noexcept -> bool {
        return id != 0;
    }

    uint32_t id = 0;
    uint32_t size = 0;
};

struct CreateBufferInfo {
    void* data = nullptr;
    size_t size = 0;
};

struct Renderbuffer {
    uint32_t id = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t samples = 0;
};

struct CreateRenderBufferInfo {
    PixelFormat format;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t samples = 0;
};

struct Framebuffer {
    auto is_valid( ) const noexcept -> bool {
        return id != static_cast<uint32_t>( -1 );
    }

    auto is_complete( ) const noexcept {
        return status == GL_FRAMEBUFFER_COMPLETE;
    }

    operator bool( ) const {
        return is_complete( ) && is_valid( );
    }

    uint32_t id = -1;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mask = 0;
    uint32_t status = 0;
};

struct FramebufferAttachment {
    uint32_t attachment = 0;
    uint32_t attachment_target = 0;
    uint32_t render_target = 0;
};

struct CreateFramebufferInfo {
    uint32_t width = 0;
    uint32_t height = 0;

    std::vector<FramebufferAttachment> attachments;
};

struct Shader {
    auto is_valid( ) const noexcept -> bool {
        return id != 0;
    }

    uint32_t id = 0;
    uint32_t target = 0;
};

enum class ShaderType {
    unknown,
    vertex, // Vertex Shader
    fragmet, // Fragment Shader
    geometry, // Geometry Shader
    tess_control, // Tessellation Control Shader
    tess_evaluation, // Tessellation Evaluation Shader
    compute // Compute Shader
};

struct CreateShaderInfo {
    ShaderType type = ShaderType::unknown;
    std::string source;
};

struct Geometry {
    auto is_valid( ) const noexcept -> bool {
        return vao != 0;
    }

    uint32_t vb = 0; // Vertex buffer
    uint32_t eb = 0; // Element buffer
    uint32_t vao = 0; // Vertex array object

    VertexFormat format = VertexFormat::unknown;
    uint32_t num_elements = 0;
};

struct CreateGeometryInfo {
    size_t vertices_num = 0;
    size_t indices_num = 0;
    vec3 min = vec3 { 0.f };
    vec3 max = vec3 { 0.f };
    VertexFormat format = VertexFormat::unknown;
    u8_buffer vertices;
    std::vector<uint16_t> indices = { };
};

struct ProgramResourceInfo {
    std::string name;
    uint32_t pid = 0;
    int32_t location = -1;
    int32_t num = 0;
    uint32_t type = 0;
    uint32_t buffer_binding = 0;
};

struct ProgramPipeline {
    auto is_valid( ) const noexcept -> bool {
        return id != 0;
    }

    uint32_t id = 0;

    std::vector<ProgramResourceInfo> uniforms;
    std::vector<ProgramResourceInfo> attributes;
    std::vector<ProgramResourceInfo> uniform_blocks;
};

struct CreatePipelineInfo {
    std::vector<Shader> shaders;
};

[[maybe_unused]] static Framebuffer default_framebuffer { .id = 0, .mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT };

///
/// Commands
///
struct ClearCommand {
    ClearCommand( ) = default;

    ClearCommand( const vec4& clear_color )
        : color { clear_color }
        , viewport { 0.f, 0.f, default_framebuffer.width, default_framebuffer.height }
        , depth { 1.f }
        , fb { default_framebuffer.id } {
    }

    ClearCommand( const Framebuffer& f, const vec4& clear_color )
        : color { clear_color }
        , viewport { 0.f, 0.f, f.width, f.height }
        , depth { 1.f }
        , fb { f.id } {
    }

    vec4 color = { 0.0f, 0.0f, 0.0f, 1.f };
    vec4 viewport = { };
    float depth = 1.0f;
    uint32_t fb = 0;
};

struct BindBufferCommand {
    BindBufferCommand( ) = default;
    BindBufferCommand( BufferType t, const Buffer& b )
        : type { t }
        , id { b.id } {
    }

    BindBufferCommand( BufferType t, const Buffer& b, const ProgramPipeline& p, std::string_view block_name )
        : type { t }
        , id { b.id }
        , offset { 0 }
        , size { b.size } {
        for ( const auto& ub : p.uniform_blocks ) {
            if ( ub.name == block_name ) {
                block_index = ub.buffer_binding;
                break;
            }
        }
    }

    BufferType type = BufferType::Unknown;
    int32_t block_index = -1;
    uint32_t id = 0;
    uint32_t offset = 0;
    uint32_t size = 0;
};

struct BindProgramCommand {
    BindProgramCommand( ) = default;
    BindProgramCommand( const ProgramPipeline& p )
        : id { p.id } {
    }

    uint32_t id = 0;
};

struct BindVertexArrayCommand {
    BindVertexArrayCommand( ) = default;
    BindVertexArrayCommand( uint32_t vao )
        : id { vao } {
    }
    BindVertexArrayCommand( const Geometry& g )
        : BindVertexArrayCommand { g.vao } {
    }

    uint32_t id = 0;
};

struct BindTextureCommand {
    uint32_t unit = 0;
    uint32_t id = 0;
    uint32_t sampler = 0;
};

struct BindFramebufferCommand {
    BindFramebufferCommand( ) = default;

    BindFramebufferCommand( const Framebuffer& fb )
        : id { fb.id } {
    }

    uint32_t id = 0;
};

struct BlitFramebufferCommand {
    BlitFramebufferCommand( ) = default;

    BlitFramebufferCommand( const Framebuffer& fbread, const Framebuffer& fbwrite )
        : src { fbread.id }
        , dst { fbwrite.id }
        , src_x { 0 }
        , src_y { 0 }
        , src_w { static_cast<int32_t>( fbread.width ) }
        , src_h { static_cast<int32_t>( fbread.height ) }
        , dst_x { 0 }
        , dst_y { 0 }
        , dst_w { static_cast<int32_t>( fbwrite.width ) }
        , dst_h { static_cast<int32_t>( fbwrite.height ) } {
    }

    uint32_t src = 0;
    uint32_t dst = 0;

    int32_t src_x = 0;
    int32_t src_y = 0;
    int32_t src_w = 0;
    int32_t src_h = 0;
    int32_t dst_x = 0;
    int32_t dst_y = 0;
    int32_t dst_w = 0;
    int32_t dst_h = 0;
};

using UniformValue = std::variant<int, float, vec2, vec3, vec4, mat4>;

struct SetUniformCommand {
    template <typename T> SetUniformCommand( const ProgramPipeline& p, const std::string_view name, const T& v ) {
        for ( const auto& u : p.uniforms ) {
            if ( u.name == name ) {
                location = u.location;
                count = u.num;
                value = v;
                pid = u.pid;
                _name = name;
                break;
            }
        }
    }

    uint32_t pid = 0;
    int32_t location = -1;
    uint32_t count = 0;
    UniformValue value;
    std::string _name;
};

struct DrawElementsCommand {
    VertexFormat format = VertexFormat::unknown;
    uint32_t mode = GL_TRIANGLES;
    uint32_t base_element = 0;
    uint32_t num_elements = 0;
    uint32_t num_instances = 1;
};

using Command = std::variant<ClearCommand, BindBufferCommand, BindProgramCommand, BindVertexArrayCommand,
    BindTextureCommand, BindFramebufferCommand, BlitFramebufferCommand, SetUniformCommand, DrawElementsCommand>;

struct CommandBuffer {
    CommandBuffer( ) = default;

    CommandBuffer( const CommandBuffer& ) = delete;
    auto operator=( const CommandBuffer& ) -> CommandBuffer& = delete;

    CommandBuffer( CommandBuffer&& a ) = default;
    auto operator=( CommandBuffer&& a ) -> CommandBuffer& = default;

    template <typename... Args> auto emplace( Args&&... args ) {
        commands.emplace( std::forward<Args>( args )... );
    }

    auto push( const Command& command ) {
        commands.push_back( command );
    }

    auto clear( ) noexcept {
        commands.clear( );
    }

    bool presentation_clear = true;

    ColorBlendState color_blend;
    RasterizerState rasterizer;
    DepthStencilState depth_stencil;

    std::vector<Command> commands;
};

using CommandQueue = CommandBuffer;
using CommandQueues = std::vector<CommandQueue*>;

inline auto operator<<( CommandQueue& cb, const Command& c ) -> CommandQueue& {
    cb.push( c );
    return cb;
}

namespace detail {

    inline auto set_color_blend_state( const ColorBlendState& state ) -> void {
        if ( state.enable ) {
            glEnable( GL_BLEND );
            glBlendFunc( state.sfactor, state.dfactor );
        }
    }

    inline auto set_rasterizer_state( const RasterizerState& state ) -> void {
        if ( state.cull_faces ) {
            glEnable( GL_CULL_FACE );
            glCullFace( state.cull_mode );
        }
    }

    inline auto set_depth_stencil_state( const DepthStencilState& state ) -> void {
        if ( state.depth_test ) {
            glEnable( GL_DEPTH_TEST );
            glDepthFunc( state.depth_func );
        }

        if ( state.depth_write ) {
            glDepthMask( GL_TRUE );
        } else {
            glDepthMask( GL_FALSE );
        }
    }

    inline auto clear_color_blend_state( [[maybe_unused]] const ColorBlendState& state ) -> void {
        glDisable( GL_BLEND );
    }

    inline auto clear_rasterizer_state( [[maybe_unused]] const RasterizerState& state ) -> void {
        glDisable( GL_CULL_FACE );
    }

    inline auto clear_depth_stencil_state( [[maybe_unused]] const DepthStencilState& state ) -> void {
        glDepthMask( GL_TRUE );
        glDisable( GL_DEPTH_TEST );
    }

    inline auto dispath_uniform( const UniformValue& u, uint32_t pid, int32_t location, uint32_t count ) {
        if ( pid == 0 )
            return;

        std::visit(
            [&]( auto&& arg ) {
                using T = std::decay_t<decltype( arg )>;
                if constexpr ( std::is_same_v<T, float> ) {
                    glProgramUniform1fv(
                        pid, location, static_cast<GLsizei>( count ), reinterpret_cast<const float*>( &arg ) );
                } else if constexpr ( std::is_same_v<T, vec2> ) {
                    glProgramUniform2fv(
                        pid, location, static_cast<GLsizei>( count ), reinterpret_cast<const float*>( &arg[0] ) );
                } else if constexpr ( std::is_same_v<T, vec3> ) {
                    glProgramUniform3fv(
                        pid, location, static_cast<GLsizei>( count ), reinterpret_cast<const float*>( &arg[0] ) );
                } else if constexpr ( std::is_same_v<T, vec4> ) {
                    glProgramUniform4fv(
                        pid, location, static_cast<GLsizei>( count ), reinterpret_cast<const float*>( &arg[0] ) );
                } else if constexpr ( std::is_same_v<T, mat4> ) {
                    glProgramUniformMatrix4fv(
                        pid, location, count, GL_FALSE, reinterpret_cast<const float*>( &arg[0][0] ) );
                }
            },
            u );
    }

    inline auto buffer_type( BufferType type ) {
        switch ( type ) {
        case BufferType::Unknown:
            break;
        case BufferType::VertexArray:
            return GL_ARRAY_BUFFER;
        case BufferType::VertexElements:
            return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::Uniform:
            return GL_UNIFORM_BUFFER;
        }

        return GL_NONE;
    }

    inline auto have_elements( VertexFormat f ) -> bool {
        return f != VertexFormat::v3_f32 && f != VertexFormat::unknown;
    }

    inline auto dispath_command( const Command& c, [[maybe_unused]] const CommandBuffer& buf ) -> void {
        std::visit(
            [&]( auto&& arg ) {
                using T = std::decay_t<decltype( arg )>;
                if constexpr ( std::is_same_v<T, ClearCommand> ) {
                    glClipControl( GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE );
                    glClearNamedFramebufferfv( arg.fb, GL_COLOR, 0, &arg.color[0] );
                    if ( buf.depth_stencil.depth_write ) {
                        glClearNamedFramebufferfv( arg.fb, GL_DEPTH, 0, &arg.depth );
                    }
                    glViewportIndexedfv( 0, &arg.viewport[0] );
                } else if constexpr ( std::is_same_v<T, BindBufferCommand> ) {
                    if ( arg.block_index == -1 ) {
                        glBindBuffer( detail::buffer_type( arg.type ), arg.id );
                    } else {
                        glBindBufferRange(
                            detail::buffer_type( arg.type ), arg.block_index, arg.id, arg.offset, arg.size );
                    }
                } else if constexpr ( std::is_same_v<T, BindProgramCommand> ) {
                    glBindProgramPipeline( arg.id );
                } else if constexpr ( std::is_same_v<T, BindVertexArrayCommand> ) {
                    glBindVertexArray( arg.id );
                } else if constexpr ( std::is_same_v<T, BindTextureCommand> ) {
                    glBindTextureUnit( arg.unit, arg.id );
                    // glBindSampler( arg.unit, arg.sampler );
                } else if constexpr ( std::is_same_v<T, BindFramebufferCommand> ) {
                    glBindFramebuffer( GL_FRAMEBUFFER, arg.id );
                } else if constexpr ( std::is_same_v<T, BlitFramebufferCommand> ) {
                    glBlitNamedFramebuffer( arg.src, arg.dst, arg.src_x, arg.src_y, arg.src_w, arg.src_h, arg.dst_x,
                        arg.dst_y, arg.dst_w, arg.dst_h, GL_COLOR_BUFFER_BIT, GL_LINEAR );
                } else if constexpr ( std::is_same_v<T, SetUniformCommand> ) {
                    dispath_uniform( arg.value, arg.pid, arg.location, arg.count );
                } else if constexpr ( std::is_same_v<T, DrawElementsCommand> ) {
                    if ( have_elements( arg.format ) ) {
                        if ( arg.num_instances == 1 ) {
                            glDrawElementsBaseVertex(
                                GL_TRIANGLES, arg.num_elements, GL_UNSIGNED_SHORT, nullptr, arg.base_element );
                        } else {
                            glDrawElementsInstancedBaseVertex( GL_TRIANGLES, arg.num_elements, GL_UNSIGNED_SHORT,
                                nullptr, arg.num_instances, arg.base_element );
                        }
                    } else {
                        if ( arg.num_instances == 1 ) {
                            glDrawArrays( arg.mode, arg.base_element, arg.num_elements );
                        } else {
                            glDrawArraysInstanced( arg.mode, arg.base_element, arg.num_elements, arg.num_instances );
                        }
                    }
                }
            },
            c );
    }

    inline auto get_program_resources( uint32_t pid, uint32_t program_interface ) {
        std::vector<ProgramResourceInfo> resources;

        if ( program_interface == GL_UNIFORM_BLOCK ) {
            GLint num_uniform_blocks = 0;
            glGetProgramInterfaceiv( pid, program_interface, GL_ACTIVE_RESOURCES, &num_uniform_blocks );

            size_t num_properties = 3;
            const GLenum properties[3] = { GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_BINDING, GL_NAME_LENGTH };

            for ( auto i = 0; i < num_uniform_blocks; i++ ) {
                GLint values[3] = { };
                glGetProgramResourceiv( pid, program_interface, i, num_properties, properties, 3, nullptr, values );

                std::string name;
                name.resize( values[2] );
                glGetProgramResourceName( pid, program_interface, i, name.size( ), nullptr, &name[0] );
                name.resize( values[2] - 1 );

                resources.push_back(
                    { .name = name, .pid = pid, .buffer_binding = static_cast<uint32_t>( values[1] ) } );
            }
        } else {
            size_t num_properties = program_interface == GL_UNIFORM ? 5 : 4;

            GLint num_uniforms = 0;
            glGetProgramInterfaceiv( pid, program_interface, GL_ACTIVE_RESOURCES, &num_uniforms );
            const GLenum properties[5] = { GL_TYPE, GL_ARRAY_SIZE, GL_NAME_LENGTH, GL_LOCATION, GL_BLOCK_INDEX };

            for ( auto i = 0; i < num_uniforms; i++ ) {
                GLint values[5] = { };
                glGetProgramResourceiv( pid, program_interface, i, num_properties, properties, 5, nullptr, values );

                if ( program_interface == GL_UNIFORM && values[4] != -1 )
                    continue;

                std::string name;
                name.resize( values[2] );
                glGetProgramResourceName( pid, program_interface, i, name.size( ), nullptr, &name[0] );
                name.resize( values[2] - 1 );

                resources.push_back( { name, pid, values[3], values[1], static_cast<uint32_t>( values[0] ) } );
            }
        }

        return resources;
    }

    inline auto get_texture_format_from_pixelformat(
        PixelFormat pf, GLint& internalformat, GLenum& format, GLenum& type ) -> void {
        switch ( pf ) {
        case PixelFormat::bgr8:
            internalformat = GL_RGB8;
            format = GL_BGR;
            type = GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::bgra8:
            internalformat = GL_RGBA8;
            format = GL_BGRA;
            type = GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::rgb8:
            internalformat = GL_RGB8;
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::rgba8:
            internalformat = GL_RGBA8;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        case PixelFormat::rgb16f:
            internalformat = GL_RGB16F;
            format = GL_RGB;
            type = GL_HALF_FLOAT;
            break;
        case PixelFormat::rgba16f:
            internalformat = GL_RGBA16F;
            format = GL_RGBA;
            type = GL_HALF_FLOAT;
            break;
        case PixelFormat::rgb32f:
            internalformat = GL_RGB32F;
            format = GL_RGB;
            type = GL_FLOAT;
            break;
        case PixelFormat::rgba32f:
            internalformat = GL_RGBA32F;
            format = GL_RGBA;
            type = GL_FLOAT;
            break;
        case PixelFormat::depth:
            internalformat = GL_DEPTH_COMPONENT32F;
            format = GL_DEPTH_COMPONENT;
            type = GL_FLOAT;
            break;
        default:
            break;
        }
    }

    inline auto apply_texture_fitering( uint32_t id, const TextureFiltering filtering, const int32_t levels ) {
        switch ( filtering ) {
        case TextureFiltering::None:
            break;
        case TextureFiltering::Bilinear:
            glTextureParameteri( id, GL_TEXTURE_BASE_LEVEL, 0 );
            glTextureParameteri( id, GL_TEXTURE_MAX_LEVEL, levels );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            break;
        case TextureFiltering::Trilinear:
            glTextureParameteri( id, GL_TEXTURE_BASE_LEVEL, 0 );
            glTextureParameteri( id, GL_TEXTURE_MAX_LEVEL, levels );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            break;
        case TextureFiltering::Anisotropic:
            glTextureParameteri( id, GL_TEXTURE_BASE_LEVEL, 0 );
            glTextureParameteri( id, GL_TEXTURE_MAX_LEVEL, levels );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

            glTextureParameterf( id, GL_TEXTURE_MAX_ANISOTROPY, 16.0f );
            break;
        }
    }

} // namespace detail

///
/// Command shortcuts
///
struct DrawGeometryCommand {
    DrawGeometryCommand( ) = default;

    DrawGeometryCommand( const BindVertexArrayCommand& bva, const DrawElementsCommand& de )
        : va { bva }
        , el { de } {
    }

    DrawGeometryCommand( const Geometry& g, uint32_t num_instances = 1 )
        : va { .id = g.vao }
        , el { .format = g.format, .num_elements = g.num_elements, .num_instances = num_instances } {
    }

    BindVertexArrayCommand va;
    DrawElementsCommand el;
};

using clear_framebuffer = ClearCommand;
using bind_vao = BindVertexArrayCommand;
using bind_buffer = BindBufferCommand;
using bind_pipeline = BindProgramCommand;
using bind_texture = BindTextureCommand;
using bind_framebuffer = BindFramebufferCommand;
using set_uniform = SetUniformCommand;
using draw_elements = DrawElementsCommand;
using draw_geometry = DrawGeometryCommand;
using blit_framebuffer = BlitFramebufferCommand;

inline auto operator<<( CommandQueue& cb, const DrawGeometryCommand& c ) -> CommandQueue& {
    cb.push( c.va );
    cb.push( c.el );
    return cb;
}

///
/// Interface
///
inline auto present( const CommandQueues& present_queue ) {
    for ( auto& q : present_queue ) {
        for ( auto& c : q->commands ) {
            detail::set_color_blend_state( q->color_blend );
            detail::set_rasterizer_state( q->rasterizer );
            detail::set_depth_stencil_state( q->depth_stencil );

            detail::dispath_command( c, *q );

            detail::clear_depth_stencil_state( q->depth_stencil );
            detail::clear_rasterizer_state( q->rasterizer );
            detail::clear_color_blend_state( q->color_blend );
        }

        if ( q->presentation_clear ) {
            q->clear( );
        }
    }
}

inline auto create_texture( const CreateTextureInfo& info ) noexcept -> Texture {
    const auto w = static_cast<GLsizei>( info.width );
    const auto h = static_cast<GLsizei>( info.height );

    auto internal_format = static_cast<GLint>( 0 );
    auto format = static_cast<GLenum>( 0 );
    auto type = static_cast<GLenum>( 0 );
    detail::get_texture_format_from_pixelformat( info.format, internal_format, format, type );

    auto id = 0u;
    glCreateTextures( GL_TEXTURE_2D, 1, &id );

    detail::apply_texture_fitering( id, info.filter, info.levels );

    glTextureStorage2D( id, info.levels, internal_format, w, h );
    if ( const auto pixels = info.pixels.empty( ) ? nullptr : reinterpret_cast<const void*>( &info.pixels[0] );
         pixels ) {
        glTextureSubImage2D( id, 0, 0, 0, w, h, format, type, pixels );
    }

    if ( info.mipmaps ) {
        glGenerateTextureMipmap( id );
    }

    return { id, GL_TEXTURE_2D, info.width, info.height, 0 };
}

inline auto create_texture_array( const CreateTextureArrayInfo& info ) noexcept -> Texture {
    const auto w = static_cast<GLsizei>( info.width );
    const auto h = static_cast<GLsizei>( info.height );
    const auto d = static_cast<GLsizei>( info.depth );

    auto internal_format = static_cast<GLint>( 0 );
    auto format = static_cast<GLenum>( 0 );
    auto type = static_cast<GLenum>( 0 );
    detail::get_texture_format_from_pixelformat( info.format, internal_format, format, type );

    auto id = 0u;
    glCreateTextures( GL_TEXTURE_2D_ARRAY, 1, &id );

    detail::apply_texture_fitering( id, info.filter, info.levels );

    glTextureStorage3D( id, info.levels, internal_format, w, h, d );
    for ( int i = 0; i < d; i++ ) {
        const auto pixels = info.pixels[i].empty( ) ? nullptr : reinterpret_cast<const void*>( &info.pixels[i][0] );
        glTextureSubImage3D( id, 0, 0, 0, i, w, h, 1, format, type, pixels );
    }

    if ( info.mipmaps ) {
        glGenerateTextureMipmap( id );
    }

    return { id, GL_TEXTURE_2D_ARRAY, info.width, info.height, info.depth };
}

inline auto create_texture_cube( const CreateTextureCubemapInfo& info ) noexcept -> Texture {
    const auto w = static_cast<GLsizei>( info.width );
    const auto h = static_cast<GLsizei>( info.height );
    const auto d = static_cast<GLsizei>( 6 );

    const auto levels = 4;

    auto internal_format = static_cast<GLint>( 0 );
    auto format = static_cast<GLenum>( 0 );
    auto type = static_cast<GLenum>( 0 );
    detail::get_texture_format_from_pixelformat( info.format, internal_format, format, type );

    auto id = 0u;
    glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &id );

    detail::apply_texture_fitering( id, info.filter, info.levels );

    glTextureStorage2D( id, levels, internal_format, w, h );

    for ( size_t face = 0; face < d; ++face ) {
        const auto pixels
            = info.pixels[face].empty( ) ? nullptr : reinterpret_cast<const void*>( &info.pixels[face][0] );
        glTextureSubImage3D( id, 0, 0, 0, face, w, h, 1, format, type, pixels );
    }

    if ( info.mipmaps ) {
        glGenerateTextureMipmap( id );
    }

    return { id, GL_TEXTURE_CUBE_MAP, info.width, info.height, 6 };
}

inline auto destroy_texture( Texture& t ) noexcept {
    glDeleteTextures( 1, &t.id );
}

inline auto create_renderbuffer( const CreateRenderBufferInfo& info ) -> Renderbuffer {
    auto id = 0u;
    glCreateRenderbuffers( 1, &id );

    auto internal_format = static_cast<GLint>( 0 );
    auto format = static_cast<GLenum>( 0 );
    auto type = static_cast<GLenum>( 0 );
    detail::get_texture_format_from_pixelformat( info.format, internal_format, format, type );

    if ( info.samples == 0 ) {
        glNamedRenderbufferStorage( id, static_cast<GLenum>( internal_format ), static_cast<GLsizei>( info.width ),
            static_cast<GLsizei>( info.height ) );
    } else {
        glNamedRenderbufferStorageMultisample( id, static_cast<GLsizei>( info.samples ),
            static_cast<GLenum>( internal_format ), static_cast<GLsizei>( info.width ),
            static_cast<GLsizei>( info.height ) );
    }

    return { id, info.width, info.height, info.samples };
}

inline auto destroy_renderbuffer( Renderbuffer& rb ) noexcept {
    glDeleteRenderbuffers( 1, &rb.id );
}

inline auto create_framebuffer( const CreateFramebufferInfo& info ) noexcept -> Framebuffer {
    auto id = 0u;
    glCreateFramebuffers( 1, &id );

    uint32_t mask = 0;

    for ( const auto& attachment : info.attachments ) {
        switch ( attachment.attachment ) {
        case GL_COLOR_ATTACHMENT0:
        case GL_COLOR_ATTACHMENT1:
        case GL_COLOR_ATTACHMENT2:
        case GL_COLOR_ATTACHMENT3:
        case GL_COLOR_ATTACHMENT4:
        case GL_COLOR_ATTACHMENT5:
        case GL_COLOR_ATTACHMENT6:
        case GL_COLOR_ATTACHMENT7:
        case GL_COLOR_ATTACHMENT8:
        case GL_COLOR_ATTACHMENT9:
        case GL_COLOR_ATTACHMENT10:
        case GL_COLOR_ATTACHMENT11:
        case GL_COLOR_ATTACHMENT12:
        case GL_COLOR_ATTACHMENT13:
        case GL_COLOR_ATTACHMENT14:
        case GL_COLOR_ATTACHMENT15:
        case GL_COLOR_ATTACHMENT16:
        case GL_COLOR_ATTACHMENT17:
        case GL_COLOR_ATTACHMENT18:
        case GL_COLOR_ATTACHMENT19:
        case GL_COLOR_ATTACHMENT20:
        case GL_COLOR_ATTACHMENT21:
        case GL_COLOR_ATTACHMENT22:
        case GL_COLOR_ATTACHMENT23:
        case GL_COLOR_ATTACHMENT24:
        case GL_COLOR_ATTACHMENT25:
        case GL_COLOR_ATTACHMENT26:
        case GL_COLOR_ATTACHMENT27:
        case GL_COLOR_ATTACHMENT28:
        case GL_COLOR_ATTACHMENT29:
        case GL_COLOR_ATTACHMENT30:
        case GL_COLOR_ATTACHMENT31:
            mask |= GL_COLOR_BUFFER_BIT;
            break;
        case GL_DEPTH_ATTACHMENT:
            mask |= GL_DEPTH_BUFFER_BIT;
            break;
        }

        if ( attachment.attachment_target == GL_TEXTURE_2D ) {
            glNamedFramebufferTexture( id, attachment.attachment, attachment.render_target, 0 );
        } else if ( attachment.attachment_target == GL_RENDERBUFFER ) {
            glNamedFramebufferRenderbuffer( id, attachment.attachment, GL_RENDERBUFFER, attachment.render_target );
        }
    }

    const auto status = glCheckNamedFramebufferStatus( id, GL_FRAMEBUFFER );

    return { id, info.width, info.height, mask, status };
}

inline auto destroy_framebuffer( Framebuffer& fb ) noexcept {
    glDeleteFramebuffers( 1, &fb.id );
}

inline auto create_shader( const CreateShaderInfo& info ) noexcept -> Shader {
    const char* sources[] = { info.source.c_str( ), nullptr };

    GLenum type = GL_NONE;
    switch ( info.type ) {
    case ShaderType::unknown:
        break;
    case ShaderType::vertex:
        type = GL_VERTEX_SHADER;
        break;
    case ShaderType::fragmet:
        type = GL_FRAGMENT_SHADER;
        break;
    case ShaderType::geometry:
        type = GL_GEOMETRY_SHADER;
        break;
    case ShaderType::tess_control:
        type = GL_TESS_CONTROL_SHADER;
        break;
    case ShaderType::tess_evaluation:
        type = GL_TESS_EVALUATION_SHADER;
        break;
    case ShaderType::compute:
        type = GL_COMPUTE_SHADER;
        break;
    }

    const auto id = glCreateShaderProgramv( type, 1, sources );

    GLint lenght = 0;
    glGetProgramiv( id, GL_INFO_LOG_LENGTH, &lenght );

    std::string program_log( static_cast<std::string::size_type>( lenght ), 0 );

    GLsizei written = 0;
    glGetProgramInfoLog( id, lenght, &written, std::data( program_log ) );
    if ( written > 0 ) {
        journal::critical( GRAPHICS_TAG, "Shader id({}) compilation failed : {}", id, program_log );
        exit( EXIT_FAILURE );
    }

    return Shader { id, type };
}

inline auto destroy_shader( Shader& shader ) noexcept -> void {
    glDeleteProgram( shader.id );
    shader.id = 0;
    shader.target = GL_NONE;
}

inline auto create_program_pipeline( const CreatePipelineInfo& info ) noexcept -> ProgramPipeline {
    auto id = 0u;
    glCreateProgramPipelines( 1, &id );

    std::vector<ProgramResourceInfo> all_uniforms;
    std::vector<ProgramResourceInfo> all_attributes;
    std::vector<ProgramResourceInfo> all_uniform_blocks;

    for ( const auto& s : info.shaders ) {
        const auto pid = s.id;
        if ( s.target == GL_VERTEX_SHADER ) { }
        switch ( s.target ) {
        case GL_VERTEX_SHADER:
            glUseProgramStages( id, GL_VERTEX_SHADER_BIT, pid );
            break;
        case GL_FRAGMENT_SHADER:
            glUseProgramStages( id, GL_FRAGMENT_SHADER_BIT, pid );
            break;
        case GL_GEOMETRY_SHADER:
            glUseProgramStages( id, GL_GEOMETRY_SHADER_BIT, pid );
            break;
        case GL_TESS_CONTROL_SHADER:
            glUseProgramStages( id, GL_TESS_CONTROL_SHADER_BIT, pid );
            break;
        case GL_TESS_EVALUATION_SHADER:
            glUseProgramStages( id, GL_TESS_EVALUATION_SHADER_BIT, pid );
            break;
        case GL_COMPUTE_SHADER:
            glUseProgramStages( id, GL_COMPUTE_SHADER_BIT, pid );
            break;
        }

        auto uniforms = detail::get_program_resources( pid, GL_UNIFORM );
        for ( const auto& u : uniforms ) {
            journal::verbose( GRAPHICS_TAG, "Uniform {}, Location {}", u.name, u.location );
        }

        auto uniform_blocks = detail::get_program_resources( pid, GL_UNIFORM_BLOCK );
        for ( const auto& ub : uniform_blocks ) {
            journal::verbose( GRAPHICS_TAG, "Uniform Block {}, Buffer binding {}", ub.name, ub.buffer_binding );
        }

        auto attributes = detail::get_program_resources( pid, GL_PROGRAM_INPUT );
        for ( const auto& a : attributes ) {
            journal::verbose( GRAPHICS_TAG, "Attribute {}, Location {}", a.name, a.location );
        }

        all_uniforms.insert( std::end( all_uniforms ), std::begin( uniforms ), std::end( uniforms ) );
        all_attributes.insert( std::end( all_attributes ), std::begin( attributes ), std::end( attributes ) );
        all_uniform_blocks.insert(
            std::end( all_uniform_blocks ), std::begin( uniform_blocks ), std::end( uniform_blocks ) );
    }

    return ProgramPipeline { id, all_uniforms, all_attributes, all_uniform_blocks };
}

inline auto destroy_program_pipeline( ProgramPipeline& p ) noexcept {
    glDeleteProgramPipelines( 1, &p.id );
    p.id = 0;
}

inline auto create_buffer( const CreateBufferInfo& info ) noexcept -> Buffer {
    auto id = 0u;
    glCreateBuffers( 1, &id );
    glNamedBufferData( id, info.size, info.data, GL_DYNAMIC_DRAW );

    return { id, static_cast<uint32_t>( info.size ) };
}

inline auto destroy_buffer( Buffer& b ) {
    glDeleteBuffers( 1, &b.id );
    b.id = 0;
}

inline auto update_buffer( Buffer& b, void* data, size_t size ) noexcept {
    void* ptr = glMapNamedBufferRange( b.id, 0, size, GL_MAP_WRITE_BIT );
    memcpy( ptr, data, size );
    glUnmapNamedBuffer( b.id );
}

template <typename T> inline auto update_buffer( Buffer& b, const std::vector<T>& data ) noexcept {
    const auto size = std::size( data ) * sizeof( T );
    void* ptr = glMapNamedBufferRange( b.id, 0, size, GL_MAP_WRITE_BIT );
    memcpy( ptr, &data[0], size );
    glUnmapNamedBuffer( b.id );
}

template <typename T, size_t N> inline auto update_buffer( Buffer& b, const std::array<T, N>& data ) noexcept {
    const auto size = std::size( data ) * sizeof( T );
    void* ptr = glMapNamedBufferRange( b.id, 0, size, GL_MAP_WRITE_BIT );
    memcpy( ptr, &data[0], size );
    glUnmapNamedBuffer( b.id );
}

inline auto create_geometry( [[maybe_unused]] const CreateGeometryInfo& info ) noexcept -> Geometry {
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint vao = 0;
    uint32_t num_elements = 0;

    switch ( info.format ) {
    case VertexFormat::unknown:
        journal::warning( GRAPHICS_TAG, "Unknown vertex format for buffer" );
        break;
    case VertexFormat::v3_f32:
        glCreateBuffers( 1, &vbo );
        glNamedBufferData( vbo, info.vertices_num * sizeof( v3_t ), &info.vertices[0], GL_STATIC_DRAW );
        break;
    case VertexFormat::v3_f32ui16:
        glCreateBuffers( 1, &vbo );
        glCreateBuffers( 1, &ebo );
        glNamedBufferData( vbo, info.vertices_num * sizeof( v3_t ), &info.vertices[0], GL_STATIC_DRAW );
        glNamedBufferData( ebo, info.indices_num * sizeof( uint16_t ), &info.indices[0], GL_STATIC_DRAW );
        break;
    case VertexFormat::v3n3_f32ui16:
        glCreateBuffers( 1, &vbo );
        glCreateBuffers( 1, &ebo );
        glNamedBufferData( vbo, info.vertices_num * sizeof( v3n3_t ), &info.vertices[0], GL_STATIC_DRAW );
        glNamedBufferData( ebo, info.indices_num * sizeof( uint16_t ), &info.indices[0], GL_STATIC_DRAW );
        break;
    case VertexFormat::v3t2_f32ui16:
        glCreateBuffers( 1, &vbo );
        glCreateBuffers( 1, &ebo );
        glNamedBufferData( vbo, info.vertices_num * sizeof( v3t2_t ), &info.vertices[0], GL_STATIC_DRAW );
        glNamedBufferData( ebo, info.indices_num * sizeof( uint16_t ), &info.indices[0], GL_STATIC_DRAW );
        break;
    case VertexFormat::v3t2n3_f32ui16:
        glCreateBuffers( 1, &vbo );
        glCreateBuffers( 1, &ebo );
        glNamedBufferData( vbo, info.vertices_num * sizeof( v3t2n3_t ), &info.vertices[0], GL_STATIC_DRAW );
        glNamedBufferData( ebo, info.indices_num * sizeof( uint16_t ), &info.indices[0], GL_STATIC_DRAW );
        break;
    case VertexFormat::v3uv2n3t3_f32ui16:
        glCreateBuffers( 1, &vbo );
        glCreateBuffers( 1, &ebo );
        glNamedBufferData( vbo, info.vertices_num * sizeof( v3uv2n3t3_t ), &info.vertices[0], GL_STATIC_DRAW );
        glNamedBufferData( ebo, info.indices_num * sizeof( uint16_t ), &info.indices[0], GL_STATIC_DRAW );
        break;
    }

    glCreateVertexArrays( 1, &vao );

    switch ( info.format ) {
    case VertexFormat::unknown:
        journal::warning( GRAPHICS_TAG, "Unknown vertex format for attributes" );
        break;
    case VertexFormat::v3_f32:
        glEnableVertexArrayAttrib( vao, 0 );
        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( v3_t, position ) );

        glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( v3_t ) );

        num_elements = info.vertices_num;
        break;
    case VertexFormat::v3_f32ui16:
        glEnableVertexArrayAttrib( vao, 0 );
        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( v3_t, position ) );

        glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( v3_t ) );
        glVertexArrayElementBuffer( vao, ebo );

        num_elements = info.indices_num;
        break;
    case VertexFormat::v3n3_f32ui16:
        glEnableVertexArrayAttrib( vao, 0 );
        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( v3n3_t, position ) );

        glEnableVertexArrayAttrib( vao, 2 );
        glVertexArrayAttribBinding( vao, 2, 0 );
        glVertexArrayAttribFormat( vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof( v3n3_t, normal ) );

        glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( v3n3_t ) );
        glVertexArrayElementBuffer( vao, ebo );

        num_elements = info.indices_num;
        break;
    case VertexFormat::v3t2_f32ui16:
        glEnableVertexArrayAttrib( vao, 0 );
        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( v3t2_t, position ) );

        glEnableVertexArrayAttrib( vao, 1 );
        glVertexArrayAttribBinding( vao, 1, 0 );
        glVertexArrayAttribFormat( vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof( v3t2_t, uv ) );

        glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( v3t2_t ) );
        glVertexArrayElementBuffer( vao, ebo );

        num_elements = info.indices_num;
        break;
    case VertexFormat::v3t2n3_f32ui16:
        glEnableVertexArrayAttrib( vao, 0 );
        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( v3t2n3_t, position ) );

        glEnableVertexArrayAttrib( vao, 1 );
        glVertexArrayAttribBinding( vao, 1, 0 );
        glVertexArrayAttribFormat( vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof( v3t2n3_t, uv ) );

        glEnableVertexArrayAttrib( vao, 2 );
        glVertexArrayAttribBinding( vao, 2, 0 );
        glVertexArrayAttribFormat( vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof( v3t2n3_t, normal ) );

        glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( v3t2n3_t ) );
        glVertexArrayElementBuffer( vao, ebo );

        num_elements = info.indices_num;
        break;
    case VertexFormat::v3uv2n3t3_f32ui16:
        glEnableVertexArrayAttrib( vao, 0 );
        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( v3uv2n3t3_t, position ) );

        glEnableVertexArrayAttrib( vao, 1 );
        glVertexArrayAttribBinding( vao, 1, 0 );
        glVertexArrayAttribFormat( vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof( v3uv2n3t3_t, uv ) );

        glEnableVertexArrayAttrib( vao, 2 );
        glVertexArrayAttribBinding( vao, 2, 0 );
        glVertexArrayAttribFormat( vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof( v3uv2n3t3_t, normal ) );

        glEnableVertexArrayAttrib( vao, 3 );
        glVertexArrayAttribBinding( vao, 3, 0 );
        glVertexArrayAttribFormat( vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( v3uv2n3t3_t, normal ) );

        glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( v3uv2n3t3_t ) );
        glVertexArrayElementBuffer( vao, ebo );

        num_elements = info.indices_num;
        break;
    }

    Geometry g;
    g.vb = vbo;
    g.eb = ebo;
    g.vao = vao;
    g.num_elements = num_elements;
    g.format = info.format;

    return g;
}

inline auto destroy_geometry( Geometry& geometry ) noexcept -> void {
    glDeleteBuffers( 1, &geometry.vb );
    geometry.vb = 0;
    glDeleteBuffers( 1, &geometry.eb );
    geometry.eb = 0;
    glDeleteVertexArrays( 1, &geometry.vao );
    geometry.vao = 0;
}

namespace extention {

    struct Image {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
        PixelFormat format = PixelFormat::unknown;
        bool mipmaps = false;
        u8_buffer pixels;
    };

    namespace detail {

        enum TARGA_DATA_TYPE {
            TARGA_DATA_NO = 0,
            TARGA_DATA_COLOR_MAPPED = 1, // indexed
            TARGA_DATA_TRUE_COLOR = 2, // RGB
            TARGA_DATA_BLACK_AND_WHITE = 3, // grayscale
            TAGRA_DATA_RLE_COLOR_MAPPED = 9,
            TARGA_DATA_RLE_TRUE_COLOR = 10,
            TARGA_DATA_RLE_BLACK_AND_WITE = 11
        };

#pragma pack( push, tga_header_align )
#pragma pack( 1 )
        typedef struct TargaHeader {
            uint8_t length;
            uint8_t color_map;
            uint8_t data_type;
            uint16_t colormap_index;
            uint16_t colormap_length;
            uint8_t colormap_entry_size;
            uint16_t x;
            uint16_t y;
            uint16_t width;
            uint16_t height;
            uint8_t bpp;
            uint8_t decription;
        } TARGA_HEADER;
#pragma pack( pop, tga_header_align )

    } // namespace detail

    inline auto load_targa( std::ifstream& fs ) -> std::optional<Image> {
        using namespace std;

        if ( !fs.is_open( ) )
            return { };

        Image image;

        fs.seekg( 0, ios::end );
        auto lenght = static_cast<size_t>( fs.tellg( ) );
        fs.seekg( 0, ios::beg );

        detail::TARGA_HEADER header;
        fs.read( reinterpret_cast<char*>( &header ), sizeof header );

        const uint8_t bytesperpixel = header.bpp / 8;
        u8_buffer data;
        data.resize( header.width * header.height * ( bytesperpixel + 1 ) );

        uint8_t* pdata = &data[0];

        if ( header.data_type == detail::TARGA_DATA_RLE_TRUE_COLOR ) {
            uint8_t block = 0;
            // size_t readen = 0;

            for ( int i = 0; i < header.width * header.height; i++ ) {
                if ( fs.read( reinterpret_cast<char*>( &block ), sizeof block ) ) {
                    uint8_t count = ( block & 0x7f ) + 1;

                    if ( block & 0x80 ) {
                        uint8_t bytes[4] = { 0 };
                        fs.read( reinterpret_cast<char*>( bytes ), bytesperpixel );

                        for ( int j = 0; j < count; j++ ) {
                            memcpy( pdata, bytes, bytesperpixel );
                            pdata += bytesperpixel;
                        }
                    } else {
                        fs.read( reinterpret_cast<char*>( pdata ), bytesperpixel * count );
                        pdata += bytesperpixel * count;
                    }
                }
            }

            fs.close( );
        } else if ( ( header.data_type == detail::TARGA_DATA_TRUE_COLOR )
            || ( header.data_type == detail::TARGA_DATA_BLACK_AND_WHITE ) ) {
            if ( !fs.read( reinterpret_cast<char*>( &data[0] ), lenght - sizeof( header ) ) ) {
                fs.close( );
            }
        }

        switch ( header.bpp ) {
        case 8:
            image.format = PixelFormat::r8;
            break;
        case 24:
            image.format = PixelFormat::bgr8;
            break;
        case 32:
            image.format = PixelFormat::bgra8;
            break;
        default:
            image.format = PixelFormat::unknown;
        }

        image.width = header.width;
        image.height = header.height;
        image.depth = 0;
        image.pixels = data;

        return { image };
    }

} // namespace extention

using extention::load_targa;

} // namespace graphics

namespace gfx = graphics;