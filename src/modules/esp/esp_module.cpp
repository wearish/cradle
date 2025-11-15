#include "esp_module.hpp"
#include "util/renderer.hpp"
#include "util/engine/visualengine/visualengine.hpp"
#include "util/engine/datamodel/datamodel.hpp"
#include "util/engine/wallcheck/wallcheck.hpp"
#include "util/engine/offsets.hpp"
#include <algorithm>

#undef min
#undef max

namespace cradle
{
    namespace modules
    {
        ESPModule::ESPModule()
            : Module("esp", "shows boxes around players")
        {
            settings.push_back(Setting("box esp", true));
            settings.push_back(Setting("name esp", false));
            settings.push_back(Setting("distance esp", false));
            settings.push_back(Setting("visible color", 0.0f, 1.0f, 0.0f, 1.0f));
            settings.push_back(Setting("hidden color", 1.0f, 0.0f, 0.0f, 1.0f));
        }

        void ESPModule::on_render()
        {
            using namespace engine;

            auto box_setting = get_setting("box esp");
            auto name_setting = get_setting("name esp");
            auto distance_setting = get_setting("distance esp");
            auto visible_color = get_setting("visible color");
            auto hidden_color = get_setting("hidden color");

            if (!box_setting || !box_setting->value.bool_val)
                return;

            auto players = PlayerCache::get_players();
            if (players.empty())
                return;

            using cradle::engine::VisualEngine;
            VisualEngine ve = VisualEngine::get_instance();
            if (!ve.is_valid())
                return;

            vector2 screen_size = ve.get_dimensions();
            matrix4 view_matrix = ve.get_viewmatrix();

            if (screen_size.X <= 0 || screen_size.Y <= 0)
                return;

            DataModel dm = DataModel::get_instance();
            if (!dm.is_valid())
                return;

            Instance camera = dm.get_current_camera();
            if (!camera.is_valid())
                return;

            vector3 camera_pos = camera.get_pos();

            ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
            auto local = PlayerCache::get_local_player();

            for (auto &player : players)
            {
                if (!player.head.is_valid() || !player.hrp.is_valid())
                    continue;

                if (local.is_valid())
                {
                    if (player.character.address == local.character.address)
                        continue;
                    if (player.hrp.address == local.hrp.address)
                        continue;
                }

                vector3 head_pos = player.head.get_pos();
                vector3 upper_torso_pos = player.upper_torso.is_valid() ? player.upper_torso.get_pos() : player.hrp.get_pos();
                vector3 hrp_pos = player.hrp.get_pos();

                std::vector<Instance> body_parts = {
                    player.head, player.upper_torso, player.left_hand, player.right_hand,
                    player.left_foot, player.right_foot};

                std::vector<vector2> screen_points;
                screen_points.reserve(body_parts.size());

                for (auto &part : body_parts)
                {
                    if (!part.is_valid())
                        continue;
                    vector2 screen_pt = world_to_screen(part.get_pos(), view_matrix, screen_size);
                    if (screen_pt.X != -1 && screen_pt.Y != -1)
                    {
                        screen_points.push_back(screen_pt);
                    }
                }

                if (screen_points.size() < 3)
                    continue;

                float min_x = screen_points[0].X, max_x = screen_points[0].X;
                float min_y = screen_points[0].Y, max_y = screen_points[0].Y;

                for (auto &pt : screen_points)
                {
                    if (pt.X < min_x)
                        min_x = pt.X;
                    if (pt.X > max_x)
                        max_x = pt.X;
                    if (pt.Y < min_y)
                        min_y = pt.Y;
                    if (pt.Y > max_y)
                        max_y = pt.Y;
                }

                vector2 head_screen = world_to_screen(head_pos, view_matrix, screen_size);
                vector2 torso_screen = world_to_screen(upper_torso_pos, view_matrix, screen_size);
                float margin = (head_screen.Y != -1 && torso_screen.Y != -1)
                                   ? std::abs(head_screen.Y - torso_screen.Y) * 0.65f
                                   : 5.0f;

                min_x -= margin;
                max_x += margin;
                min_y -= margin;
                max_y += margin;

                float distance = std::sqrt(
                    (camera_pos.X - hrp_pos.X) * (camera_pos.X - hrp_pos.X) +
                    (camera_pos.Y - hrp_pos.Y) * (camera_pos.Y - hrp_pos.Y) +
                    (camera_pos.Z - hrp_pos.Z) * (camera_pos.Z - hrp_pos.Z));

                vector3 torso_pos = hrp_pos;
                vector3 pelvis_pos = hrp_pos + vector3(0, -1, 0);
                vector3 left_foot_pos = hrp_pos + vector3(-0.5f, -2, 0);
                vector3 right_foot_pos = hrp_pos + vector3(0.5f, -2, 0);

                bool visible = Wallcheck::is_cache_ready() &&
                               Wallcheck::is_visible(camera_pos, head_pos, torso_pos, pelvis_pos, left_foot_pos, right_foot_pos, player.character.address);

                ImU32 box_color;
                if (visible && visible_color)
                {
                    box_color = IM_COL32(
                        (int)(visible_color->value.color_val[0] * 255),
                        (int)(visible_color->value.color_val[1] * 255),
                        (int)(visible_color->value.color_val[2] * 255),
                        (int)(visible_color->value.color_val[3] * 255));
                }
                else if (!visible && hidden_color)
                {
                    box_color = IM_COL32(
                        (int)(hidden_color->value.color_val[0] * 255),
                        (int)(hidden_color->value.color_val[1] * 255),
                        (int)(hidden_color->value.color_val[2] * 255),
                        (int)(hidden_color->value.color_val[3] * 255));
                }
                else
                {
                    box_color = visible ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
                }

                draw_list->AddRect(
                    ImVec2(min_x, min_y),
                    ImVec2(max_x, max_y),
                    box_color, 0.0f, 0, 1.0f);

                if (name_setting && name_setting->value.bool_val && !player.name.empty())
                {
                    ImVec2 text_size = ImGui::CalcTextSize(player.name.c_str());
                    float text_x = min_x + (max_x - min_x - text_size.x) / 2.0f;
                    float text_y = min_y - text_size.y - 2;

                    draw_list->AddText(
                        ImVec2(text_x + 1, text_y + 1),
                        IM_COL32(0, 0, 0, 255),
                        player.name.c_str());
                    draw_list->AddText(
                        ImVec2(text_x, text_y),
                        IM_COL32(255, 255, 255, 255),
                        player.name.c_str());
                }

                if (distance_setting && distance_setting->value.bool_val)
                {
                    char dist_text[32];
                    snprintf(dist_text, sizeof(dist_text), "%.0fm", distance);

                    ImVec2 text_size = ImGui::CalcTextSize(dist_text);
                    float text_x = min_x + (max_x - min_x - text_size.x) / 2.0f;
                    float text_y = max_y + 2;

                    draw_list->AddText(
                        ImVec2(text_x + 1, text_y + 1),
                        IM_COL32(0, 0, 0, 255),
                        dist_text);
                    draw_list->AddText(
                        ImVec2(text_x, text_y),
                        IM_COL32(255, 255, 255, 255),
                        dist_text);
                }
            }
        }
    }
}
