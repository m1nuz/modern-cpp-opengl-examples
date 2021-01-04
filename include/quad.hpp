#pragma once

#include <cstdint>

constexpr uint32_t QUAD_NUM_VERTICES = 4;
constexpr uint32_t QUAD_NUM_INDICES = 6;

namespace {
namespace detail {
    struct v3t2_t {
        float position[3];
        float texcoord[2];
    };
} // namespace detail
} // namespace

constexpr ::detail::v3t2_t quad_vertices[QUAD_NUM_VERTICES] = {
    //
    { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } }, // 1
    { { +1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }, // 2
    { { -1.0f, +1.0f, 0.0f }, { 0.0f, 1.0f } }, // 3
    { { +1.0f, +1.0f, 0.0f }, { 1.0f, 1.0f } } // 4
};

constexpr uint16_t quad_indices[QUAD_NUM_INDICES] = { 0, 1, 2, 1, 3, 2 };