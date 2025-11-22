#pragma once
#include "../util/engine/instance/instance.hpp"
#include <string>

namespace cradle::engine {
    
    struct Player {
        std::string name;
        std::string team;
        
        Instance character;
        Instance humanoid;
        Instance head;
        Instance hrp;
        Instance torso;
        Instance upper_torso;
        Instance lower_torso;
        Instance left_upper_arm;
        Instance right_upper_arm;
        Instance left_lower_arm;
        Instance right_lower_arm;
        Instance left_hand;
        Instance right_hand;
        Instance left_upper_leg;
        Instance right_upper_leg;
        Instance left_lower_leg;
        Instance right_lower_leg;
        Instance left_foot;
        Instance right_foot;
        
        float health = 100.0f;
        float max_health = 100.0f;
        int rig_type = 0;
        
        bool is_valid() const {
            return character.is_valid() && hrp.is_valid() && head.is_valid();
        }
    };
}
