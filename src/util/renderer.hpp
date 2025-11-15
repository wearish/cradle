#pragma once
#include "engine/math.hpp"

namespace cradle
{

    inline engine::vector2 world_to_screen(const engine::vector3 &world_pos, const engine::matrix4 &view_matrix, const engine::vector2 &screen_size)
    {
        const float *m = view_matrix.data;

        float clip_x = (world_pos.X * m[0]) + (world_pos.Y * m[1]) + (world_pos.Z * m[2]) + m[3];
        float clip_y = (world_pos.X * m[4]) + (world_pos.Y * m[5]) + (world_pos.Z * m[6]) + m[7];
        float clip_w = (world_pos.X * m[12]) + (world_pos.Y * m[13]) + (world_pos.Z * m[14]) + m[15];

        if (clip_w < 0.1f)
            return {-1, -1};

        float ndc_x = clip_x / clip_w;
        float ndc_y = clip_y / clip_w;

        return {
            (ndc_x + 1.0f) * (screen_size.X / 2.0f),
            (1.0f - ndc_y) * (screen_size.Y / 2.0f)};
    }
}
