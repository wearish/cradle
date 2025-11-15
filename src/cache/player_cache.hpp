#pragma once
#include "player.hpp"
#include "../util/engine/datamodel/datamodel.hpp"
#include <vector>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <chrono>

namespace cradle::engine
{

    class PlayerCache
    {
    private:
        static std::vector<Player> players;
        static std::unordered_map<std::uint64_t, Player> entity_map;
        static std::mutex mtx;
        static std::atomic<bool> updating;
        static std::chrono::steady_clock::time_point last_tick;
    public:
        static void update_cache();

        static std::vector<Player> get_players();
        static Player get_local_player();
        static bool is_updating() { return updating.load(); }
    };
}
