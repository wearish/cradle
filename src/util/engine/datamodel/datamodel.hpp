#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <cstdint>
#include "../instance/instance.hpp"
#include "../offsets.hpp"

namespace cradle::engine
{
    class DataModel : public Instance
    {
    public:
        DataModel() = default;
        DataModel(std::uintptr_t addr) : Instance(addr) {}

        static DataModel get_instance();

        std::uint64_t get_place_id();
        std::uint64_t get_game_id();
        bool get_game_loaded();
        std::string get_server_ip();
        std::uint64_t get_place_version();
        std::uint64_t get_creator_id();
        int get_primitive_count();

        Instance get_workspace();
        Instance get_players();
        Instance get_script_context();
        Instance get_current_camera();
    };
}
