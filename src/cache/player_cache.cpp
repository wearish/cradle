#include "player_cache.hpp"
#include "../util/engine/offsets.hpp"
#include <thread>

namespace cradle::engine
{

    std::vector<Player> PlayerCache::players;
    std::unordered_map<std::uint64_t, Player> PlayerCache::entity_map;
    std::mutex PlayerCache::mtx;
    std::atomic<bool> PlayerCache::updating{false};
    std::chrono::steady_clock::time_point PlayerCache::last_tick = std::chrono::steady_clock::now();

    static Player build_player(Instance &char_inst, Instance & /*player_inst*/)
    {
        Player p;
        if (!char_inst.is_valid())
            return p;

        p.character = char_inst;
        p.humanoid = char_inst.find_first_child_of_class("Humanoid");
        p.head = char_inst.find_first_child("Head");

        if (p.humanoid.is_valid())
            p.rig_type = p.humanoid.get_rig_type();

        if (p.rig_type == 0)
        {
            p.hrp = char_inst.find_first_child("Torso");
        }
        else
        {
            p.hrp = char_inst.find_first_child("HumanoidRootPart");
            if (!p.hrp.is_valid())
                p.hrp = char_inst.find_first_child("Hitbox");
            if (!p.hrp.is_valid())
                p.hrp = char_inst.find_first_child("UpperTorso");
        }

        if (p.rig_type == 1)
        {
            p.upper_torso = char_inst.find_first_child("UpperTorso");
            p.lower_torso = char_inst.find_first_child("LowerTorso");
            p.right_upper_arm = char_inst.find_first_child("RightUpperArm");
            p.right_lower_arm = char_inst.find_first_child("RightLowerArm");
            p.right_hand = char_inst.find_first_child("RightHand");
            p.left_upper_arm = char_inst.find_first_child("LeftUpperArm");
            p.left_lower_arm = char_inst.find_first_child("LeftLowerArm");
            p.left_hand = char_inst.find_first_child("LeftHand");
            p.right_upper_leg = char_inst.find_first_child("RightUpperLeg");
            p.right_lower_leg = char_inst.find_first_child("RightLowerLeg");
            p.right_foot = char_inst.find_first_child("RightFoot");
            p.left_upper_leg = char_inst.find_first_child("LeftUpperLeg");
            p.left_lower_leg = char_inst.find_first_child("LeftLowerLeg");
            p.left_foot = char_inst.find_first_child("LeftFoot");
        }
        else
        {
            p.torso = p.upper_torso = char_inst.find_first_child("Torso");
            p.left_hand = char_inst.find_first_child("Left Arm");
            p.right_hand = char_inst.find_first_child("Right Arm");
            p.left_foot = char_inst.find_first_child("Left Leg");
            p.right_foot = char_inst.find_first_child("Right Leg");
        }

        if (p.humanoid.is_valid())
        {
            p.health = p.humanoid.get_health();
            p.max_health = p.humanoid.get_max_health();
        }
        else
        {
            p.health = p.max_health = 100.0f;
        }

        return p;
    }

    void PlayerCache::update_cache()
    {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count() < 100)
            return;

        std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
        if (!lock.owns_lock() || updating.load())
            return;

        updating = true;

        DataModel dm = DataModel::get_instance();
        if (!dm.is_valid())
        {
            updating = false;
            return;
        }

        Instance players_service = dm.get_players();
        if (!players_service.is_valid())
        {
            updating = false;
            return;
        }

        auto children = players_service.get_children();
        if (children.size() > 50)
            entity_map.clear();

        std::vector<Player> snapshot;
        snapshot.reserve(50);

        for (Instance &player : children)
        {
            if (!player.is_valid() || player.get_class_name() != "Player")
                continue;

            Instance character = player.get_character();
            if (!character.is_valid())
                continue;

            std::string player_name = player.get_name();
            auto cached = entity_map.find(character.address);

            if (cached != entity_map.end())
            {
                cached->second.name = player_name;
                snapshot.push_back(cached->second);
                continue;
            }

            Player entity = build_player(character, player);
            if (entity.is_valid())
            {
                entity.name = player_name;
                entity_map[character.address] = entity;
                snapshot.push_back(entity);
            }
        }

        players = std::move(snapshot);
        last_tick = std::chrono::steady_clock::now();

        updating = false;
    }

    std::vector<Player> PlayerCache::get_players()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return players;
    }

    Player PlayerCache::get_local_player()
    {
        std::lock_guard<std::mutex> lock(mtx);
        DataModel dm = DataModel::get_instance();
        if (!dm.is_valid())
            return Player();

        Instance players_svc = dm.get_players();
        if (!players_svc.is_valid())
            return Player();

        Instance local = players_svc.get_local_player();
        if (!local.is_valid())
            return Player();

        Instance character = local.get_character();
        if (!character.is_valid())
            return Player();

        Player p = build_player(character, local);
        p.name = local.get_name();
        return p;
    }
}
