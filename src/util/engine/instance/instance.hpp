#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include "../offsets.hpp"
#include "../../memory/memory.hpp"
#include "../math.hpp"

namespace cradle::engine
{
    class Instance
    {
    public:
        std::uintptr_t address = 0;

        Instance() = default;
        Instance(std::uintptr_t addr) : address(addr) {}

        std::string get_name();
        std::string get_class_name() const;
        std::vector<Instance> get_children();

        Instance find_first_child(const std::string &name);
        Instance find_first_child_of_class(const std::string &class_name);
        std::vector<Instance> find_descendants_of_class(const std::string &class_name);
        Instance get_parent();
        bool is_descendant_of(const Instance &ancestor) const;

        vector3 get_pos() const;
        cframe get_cframe() const;

        Instance get_character() const;
        Instance get_local_player() const;
        float get_health() const;
        float get_max_health() const;
        int get_rig_type() const;
        std::string get_team() const;

        bool is_valid() const { return address > 0x10000; }
    };
}
