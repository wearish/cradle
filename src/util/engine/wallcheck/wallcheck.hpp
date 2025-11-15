#pragma once
#include "../instance/instance.hpp"
#include "../datamodel/datamodel.hpp"
#include "../math.hpp"
#include "../../memory/memory.hpp"
#include "../offsets.hpp"
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <cfloat>
#include <unordered_map>

namespace cradle::engine
{
    struct WorldPart
    {
        vector3 pos;
        vector3 size;
        matrix3 rot;
        float vol;
        bool large;
    };

    class Wallcheck
    {
    private:
        static std::vector<WorldPart> geometry;
        static double last_refresh;
        static std::atomic<bool> building;
        static std::atomic<bool> ready;
        static std::unordered_map<std::uint64_t, std::pair<bool, std::chrono::steady_clock::time_point>> vis_cache;
        static std::mutex geometry_mtx;
        static std::mutex vis_cache_mtx;

    public:
        static void update_world_cache(DataModel &dm);
        static bool is_visible(const vector3 &from, const vector3 &head, const vector3 &torso,
                               const vector3 &pelvis, const vector3 &left_foot, const vector3 &right_foot,
                               std::uint64_t player_addr = 0);
        static bool is_cache_ready() { return ready && !geometry.empty(); }
        static void force_cache_refresh() { last_refresh = 0.0; }
        static const std::vector<WorldPart> &get_world_parts() { return geometry; }
    };
}
