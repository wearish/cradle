#include "aimbot_module.hpp"
#include "cache/player_cache.hpp"
#include "util/engine/visualengine/visualengine.hpp"
#include "util/engine/datamodel/datamodel.hpp"
#include "util/engine/wallcheck/wallcheck.hpp"
#include "util/renderer.hpp"
#include "overlay/overlay.hpp"
#include <imgui.h>
#include <Windows.h>
#include <cmath>

using namespace cradle::engine;

namespace cradle::modules
{

    AimbotModule::AimbotModule() : Module("aimbot", "locks onto closest player")
    {
        keybind_mode = KeybindMode::HOLD;

        settings.push_back(Setting("fov size", 100.0f, 10.0f, 500.0f));
        settings.push_back(Setting("fov circle", true));
        settings.push_back(Setting("wall check", true));
        settings.push_back(Setting("team check", true));
        settings.push_back(Setting("smoothness", 0.0f, 0.0f, 0.95f));
        settings.push_back(Setting("fov color", 1.0f, 1.0f, 1.0f, 1.0f));
    }

    void AimbotModule::on_update()
    {
            if (overlay::Overlay::is_menu_open())
                return;
            if (PlayerCache::is_updating())
                return;

            auto fov_size_setting = get_setting("fov size");
            auto wall_check_setting = get_setting("wall check");
            auto team_check_setting = get_setting("team check");
            auto smoothness_setting = get_setting("smoothness");

            if (!fov_size_setting || !wall_check_setting || !team_check_setting || !smoothness_setting)
                return;

            float fov_size = fov_size_setting->value.float_val;
            bool wall_check = wall_check_setting->value.bool_val;
            bool team_check = team_check_setting->value.bool_val;
            float smoothness = smoothness_setting->value.float_val;

            auto players = PlayerCache::get_players();
            if (players.empty())
                return;

            auto local = PlayerCache::get_local_player();
            if (!local.is_valid() || !local.hrp.is_valid())
                return;

            std::string local_team = local.team;

            using cradle::engine::VisualEngine;
            VisualEngine ve = VisualEngine::get_instance();
            if (!ve.is_valid())
                return;

            matrix4 vm = ve.get_viewmatrix();
            vector2 screen_size((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));

            POINT cursor_pos;
            GetCursorPos(&cursor_pos);

            HWND roblox_window = FindWindowA(nullptr, "Roblox");
            POINT cursor_client = cursor_pos;
            if (roblox_window)
            {
                ScreenToClient(roblox_window, &cursor_client);
            }

            DataModel dm = DataModel::get_instance();
            if (!dm.is_valid())
                return;

            Instance camera = dm.get_current_camera();
            if (!camera.is_valid())
                return;

            vector3 camera_pos = camera.get_pos();

            if (wall_check)
            {
                if (!Wallcheck::is_cache_ready())
                    return;
            }

            bool found_target = false;
            Player closest_player;
            float closest_dist = FLT_MAX;

            for (const auto &p : players)
            {
                if (!p.is_valid() || !p.head.is_valid())
                    continue;
                if (!p.character.is_valid() || !p.hrp.is_valid())
                    continue;

                if (p.character.address == local.character.address)
                    continue;
                if (p.hrp.address == local.hrp.address)
                    continue;
                if (p.health <= 0.0f)
                    continue;

                if (team_check && !p.team.empty() && !local_team.empty() && p.team == local_team)
                    continue;
                vector3 head_pos = p.head.get_pos();

                if (wall_check)
                {
                    if (!Wallcheck::is_cache_ready())
                        continue;
                    if (!Wallcheck::is_visible(camera_pos, head_pos, head_pos, head_pos, head_pos, head_pos, p.character.address))
                        continue;
                }

                vector3 target_pos = p.head.is_valid() ? p.head.get_cframe().position : vector3();
                if (target_pos.X == 0 && target_pos.Y == 0 && target_pos.Z == 0)
                    continue;

                auto screen_pos = world_to_screen(target_pos, vm, screen_size);
                if (screen_pos.X < 0 || screen_pos.Y < 0)
                    continue;

                float dx = screen_pos.X - cursor_pos.x;
                float dy = screen_pos.Y - cursor_pos.y;
                float dist_2d = std::sqrt(dx * dx + dy * dy);

                if (dist_2d > fov_size)
                    continue;

                if (dist_2d < closest_dist)
                {
                    closest_dist = dist_2d;
                    closest_player = p;
                    found_target = true;
                }
            }

            if (!found_target)
                return;
            if (!closest_player.is_valid() || !closest_player.character.is_valid())
                return;
            if (!closest_player.head.is_valid())
                return;

            vector3 target_pos = closest_player.head.is_valid() ? closest_player.head.get_cframe().position : vector3();
            if (target_pos.X == 0 && target_pos.Y == 0 && target_pos.Z == 0)
                return;

            if (wall_check)
            {
                if (!Wallcheck::is_cache_ready())
                    return;
                if (!Wallcheck::is_visible(camera_pos, target_pos, target_pos, target_pos, target_pos, target_pos, closest_player.character.address))
                    return;
            }

            vector2 dimensions = ve.get_dimensions();
            float center_x = dimensions.X / 2.0f;
            float center_y = dimensions.Y / 2.0f;

            bool y_axis_check;
            if (dimensions.X < GetSystemMetrics(SM_CXSCREEN) || dimensions.Y < GetSystemMetrics(SM_CYSCREEN))
            {
                y_axis_check = (cursor_client.y - center_y) <= 25;
            }
            else
            {
                y_axis_check = (cursor_client.y == (int)center_y);
            }

            bool cursor_locked = (cursor_client.x == (int)center_x && y_axis_check);

            if (cursor_locked)
            {
                vector3 direction = (target_pos - camera_pos).normalize();
                vector3 up(0, 1, 0);
                vector3 right = direction.cross(up).normalize();
                vector3 real_up = right.cross(direction).normalize();

                matrix3 target_rot;
                target_rot.data[0] = right.X;
                target_rot.data[1] = real_up.X;
                target_rot.data[2] = -direction.X;
                target_rot.data[3] = right.Y;
                target_rot.data[4] = real_up.Y;
                target_rot.data[5] = -direction.Y;
                target_rot.data[6] = right.Z;
                target_rot.data[7] = real_up.Z;
                target_rot.data[8] = -direction.Z;

                if (smoothness > 0.0f)
                {
                    matrix3 current_rot = cradle::memory::read<matrix3>(camera.address + Offsets::Camera::Rotation);

                    float t = 1.0f - smoothness;

                    matrix3 smoothed_rot;
                    for (int i = 0; i < 9; i++)
                    {
                        smoothed_rot.data[i] = current_rot.data[i] + (target_rot.data[i] - current_rot.data[i]) * t;
                    }

                    cradle::memory::write<matrix3>(camera.address + Offsets::Camera::Rotation, smoothed_rot);
                }
                else
                {
                    cradle::memory::write<matrix3>(camera.address + Offsets::Camera::Rotation, target_rot);
                }
            }
            else
            {
                auto target_screen = world_to_screen(target_pos, vm, screen_size);

                if (target_screen.X > 0 && target_screen.Y > 0)
                {
                    float dx = target_screen.X - cursor_pos.x;
                    float dy = target_screen.Y - cursor_pos.y;

                    dx *= (1.0f - smoothness);
                    dy *= (1.0f - smoothness);

                    if (!(std::abs(dx) < 0.5f && std::abs(dy) < 0.5f))
                    {
                        int new_x = cursor_pos.x + (int)dx;
                        int new_y = cursor_pos.y + (int)dy;
                        SetCursorPos(new_x, new_y);
                    }

                    }
                }
            }

    void AimbotModule::on_render()
    {
        auto show_fov_setting = get_setting("fov circle");
        if (!show_fov_setting || !show_fov_setting->value.bool_val)
            return;

        auto fov_size_setting = get_setting("fov size");
        auto fov_color_setting = get_setting("fov color");

        if (!fov_size_setting || !fov_color_setting)
            return;

        float fov_size = fov_size_setting->value.float_val;

        ImU32 fov_color = IM_COL32(
            (int)(fov_color_setting->value.color_val[0] * 255),
            (int)(fov_color_setting->value.color_val[1] * 255),
            (int)(fov_color_setting->value.color_val[2] * 255),
            (int)(fov_color_setting->value.color_val[3] * 255));

        POINT cursor_pos;
        GetCursorPos(&cursor_pos);

        ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
        if (draw_list)
        {
            draw_list->AddCircle(ImVec2((float)cursor_pos.x, (float)cursor_pos.y), fov_size, fov_color, 64, 2.0f);
        }
    }
}
