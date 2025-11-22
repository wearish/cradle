#include "triggerbot_module.hpp"
#include "cache/player_cache.hpp"
#include "util/engine/visualengine/visualengine.hpp"
#include "util/engine/datamodel/datamodel.hpp"
#include "util/engine/wallcheck/wallcheck.hpp"
#include "util/renderer.hpp"
#include "overlay/overlay.hpp"
#include <Windows.h>
#include <cmath>

using namespace cradle::engine;

namespace cradle::modules
{
    TriggerbotModule::TriggerbotModule() : Module("triggerbot", "auto clicks when hovering over enemy")
    {
        keybind_mode = KeybindMode::HOLD;

        settings.push_back(Setting("delay (ms)", 0, 0, 500));
        settings.push_back(Setting("wall check", true));
        settings.push_back(Setting("team check", true));
        settings.push_back(Setting("head only", false));
    }

    void TriggerbotModule::on_update()
    {
        if (overlay::Overlay::is_menu_open())
            return;
        if (PlayerCache::is_updating())
            return;

        auto delay_setting = get_setting("delay (ms)");
        auto wall_check_setting = get_setting("wall check");
        auto team_check_setting = get_setting("team check");
        auto head_only_setting = get_setting("head only");

        if (!delay_setting || !wall_check_setting || !team_check_setting || !head_only_setting)
            return;

        int delay_ms = delay_setting->value.int_val;
        bool wall_check = wall_check_setting->value.bool_val;
        bool team_check = team_check_setting->value.bool_val;
        bool head_only = head_only_setting->value.bool_val;

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
        vector2 screen_size = ve.get_dimensions();

        if (screen_size.X <= 0 || screen_size.Y <= 0)
            return;

        DataModel dm = DataModel::get_instance();
        if (!dm.is_valid())
            return;

        Instance camera = dm.get_current_camera();
        if (!camera.is_valid())
            return;

        vector3 camera_pos = camera.get_pos();
        matrix3 camera_rot = camera.get_cframe().rotation;

        if (wall_check && !Wallcheck::is_cache_ready())
            return;

        float center_x = screen_size.X / 2.0f;
        float center_y = screen_size.Y / 2.0f;

        vector3 forward(-camera_rot.data[2], -camera_rot.data[5], -camera_rot.data[8]);
        forward = forward.normalize();

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

            std::vector<Instance> parts_to_check;
            if (head_only)
            {
                parts_to_check.push_back(p.head);
            }
            else
            {
                parts_to_check = {p.head, p.upper_torso, p.hrp};
                if (p.torso.is_valid())
                    parts_to_check.push_back(p.torso);
            }

            bool hovering = false;

            for (const auto &part : parts_to_check)
            {
                if (!part.is_valid())
                    continue;

                vector3 part_pos = part.get_pos();

                if (wall_check)
                {
                    if (!Wallcheck::is_visible(camera_pos, part_pos, part_pos, part_pos, part_pos, part_pos, p.character.address))
                        continue;
                }

                vector2 screen_pos = world_to_screen(part_pos, vm, screen_size);
                if (screen_pos.X < 0 || screen_pos.Y < 0)
                    continue;

                float dx = screen_pos.X - center_x;
                float dy = screen_pos.Y - center_y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < 15.0f)
                {
                    hovering = true;
                    break;
                }
            }

            if (hovering)
            {
                if (delay_ms > 0)
                    Sleep(delay_ms);

                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                Sleep(10);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                return;
            }
        }
    }
}
