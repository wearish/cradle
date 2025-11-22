#include "wallcheck.hpp"
#include <cmath>

namespace cradle::engine
{
    std::vector<WorldPart> Wallcheck::geometry;
    double Wallcheck::last_refresh = 0.0;
    std::atomic<bool> Wallcheck::building{false};
    std::atomic<bool> Wallcheck::ready{false};
    std::unordered_map<std::uint64_t, std::pair<bool, std::chrono::steady_clock::time_point>> Wallcheck::vis_cache;
    std::mutex Wallcheck::geometry_mtx;
    std::mutex Wallcheck::vis_cache_mtx;

    static inline bool check_ray_box(const vector3 &origin, const vector3 &dir, const WorldPart &part, float dist)
    {
        vector3 to_part = part.pos - origin;
        float radius = part.size.magnitude() * 0.866f;
        float dist_sq = to_part.X * to_part.X + to_part.Y * to_part.Y + to_part.Z * to_part.Z;
        if (dist_sq > (radius + dist) * (radius + dist))
            return false;

        matrix3 inv = part.rot.transpose();
        vector3 local_o = inv.multiply(origin - part.pos);
        vector3 local_d = inv.multiply(dir);

        vector3 half = part.size * 0.5f;
        float tmin = -FLT_MAX, tmax = FLT_MAX;

        for (int i = 0; i < 3; i++)
        {
            float o = (i == 0) ? local_o.X : (i == 1) ? local_o.Y
                                                      : local_o.Z;
            float d = (i == 0) ? local_d.X : (i == 1) ? local_d.Y
                                                      : local_d.Z;
            float h = (i == 0) ? half.X : (i == 1) ? half.Y
                                                   : half.Z;

            if (std::fabs(d) < 1e-6f)
            {
                if (o < -h || o > h)
                    return false;
            }
            else
            {
                float t1 = (-h - o) / d;
                float t2 = (h - o) / d;
                if (t1 > t2)
                    std::swap(t1, t2);
                if (t1 > tmin)
                    tmin = t1;
                if (t2 < tmax)
                    tmax = t2;
                if (tmin > tmax || tmax < 0.0f || tmin > dist)
                    return false;
            }
        }
        return (tmin > 0.0f || tmax > 0.0f) && tmin <= dist && tmin <= tmax;
    }

    void Wallcheck::update_world_cache(DataModel &dm)
    {
        if (!building)
        {
            building = true;
            std::uint64_t dm_addr = dm.address;

            std::thread([dm_addr]() {
                    DataModel dm_local(dm_addr);
                    std::vector<WorldPart> parts;
                    parts.reserve(3000);

                    Instance workspace = dm_local.get_workspace();
                    if (!workspace.is_valid()) {
                        std::lock_guard<std::mutex> lk(Wallcheck::geometry_mtx);
                        Wallcheck::geometry.clear();
                        ready = true;
                        building = false;
                        return;
                    }

                    Instance players = dm_local.get_players();
                    std::vector<Instance> player_characters;
                    if (players.is_valid()) {
                        for (auto &player : players.get_children()) {
                            std::uint64_t char_addr = cradle::memory::read<std::uint64_t>(player.address + Offsets::Player::ModelInstance);
                            if (char_addr > 0x10000) {
                                player_characters.push_back(Instance(char_addr));
                            }
                        }
                    }

                    std::string classes[] = {"Part", "MeshPart", "UnionOperation"};

                    for (const auto &cls : classes) {
                        for (auto &part : workspace.find_descendants_of_class(cls)) {
                            if (!part.is_valid()) continue;

                            bool is_player_part = false;
                            for (auto &character : player_characters) {
                                if (part.is_descendant_of(character)) {
                                    is_player_part = true;
                                    break;
                                }
                            }
                            if (is_player_part) continue;

                            std::uint64_t primitive = cradle::memory::read<std::uint64_t>(part.address + Offsets::BasePart::Primitive);
                            if (primitive < 0x10000) continue;

                            float transparency = cradle::memory::read<float>(primitive + Offsets::BasePart::Transparency);
                            if (transparency > 0.9f) continue;

                            vector3 size = cradle::memory::read<vector3>(primitive + Offsets::BasePart::Size);
                            float vol = size.X * size.Y * size.Z;
                            if (vol < 0.5f || vol > 8000000.0f) continue;

                            cframe cf = cradle::memory::read<cframe>(primitive + Offsets::BasePart::Rotation);
                            parts.push_back({cf.position, size, cf.rotation, vol, vol > 10.0f});
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lk(Wallcheck::geometry_mtx);
                        Wallcheck::geometry = std::move(parts);
                        Wallcheck::last_refresh = std::chrono::duration_cast<std::chrono::duration<double>>(
                            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                    }
                    {
                        std::lock_guard<std::mutex> lk2(Wallcheck::vis_cache_mtx);
                        Wallcheck::vis_cache.clear();
                    }

                    ready = true;
                    building = false;
            
            }).detach();
        }
    }

    bool Wallcheck::is_visible(const vector3 &from, const vector3 &head, const vector3 &torso,
                               const vector3 &pelvis, const vector3 &left_foot, const vector3 &right_foot,
                               std::uint64_t player_addr)
    {
        if (!ready) return true;

        std::vector<WorldPart> local_geometry;
        {
            std::lock_guard<std::mutex> lk(geometry_mtx);
            local_geometry = geometry;
        }

        if (local_geometry.empty()) return true;

        {
            if (player_addr != 0) {
                std::lock_guard<std::mutex> vlk(vis_cache_mtx);
                auto cached = vis_cache.find(player_addr);
                if (cached != vis_cache.end()) {
                    auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - cached->second.second).count();
                    if (age < 100) return cached->second.first;
                }
            }

            struct Ray { vector3 target; vector3 dir; float dist; };
            Ray rays[5] = {
                {head, (head - from).normalize(), (head - from).magnitude()},
                {torso, (torso - from).normalize(), (torso - from).magnitude()},
                {pelvis, (pelvis - from).normalize(), (pelvis - from).magnitude()},
                {left_foot, (left_foot - from).normalize(), (left_foot - from).magnitude()},
                {right_foot, (right_foot - from).normalize(), (right_foot - from).magnitude()}
            };
            std::sort(std::begin(rays), std::end(rays), [](const Ray &a, const Ray &b) { return a.dist < b.dist; });

            int visible_count = 0;
            int checked = 0;

            for (const auto &ray : rays) {
                if (checked >= 5) break;
                if (ray.dist <= 0.0f) {
                    checked++;
                    continue;
                }
                
                bool clear = true;

                for (const auto &part : local_geometry) {
                    if (!part.large && part.vol < 1.0f) continue;
                    if (part.vol <= 0.0f || part.size.X <= 0.0f || part.size.Y <= 0.0f || part.size.Z <= 0.0f) continue;

                    if (check_ray_box(from, ray.dir, part, ray.dist)) {
                        float part_dist = (part.pos - from).magnitude();
                        if (std::fabs(part_dist - ray.dist) < 1.5f) continue;
                        clear = false;
                        break;
                    }
                }

                if (clear && ++visible_count >= 2) {
                    if (player_addr != 0) {
                        std::lock_guard<std::mutex> vlk(vis_cache_mtx);
                        vis_cache[player_addr] = {true, std::chrono::steady_clock::now()};
                    }
                    return true;
                }
                checked++;
            }

            bool result = visible_count >= 2;

            if (player_addr != 0) {
                std::lock_guard<std::mutex> vlk(vis_cache_mtx);
                vis_cache[player_addr] = {result, std::chrono::steady_clock::now()};
                if (vis_cache.size() > 100) {
                    auto now = std::chrono::steady_clock::now();
                    for (auto it = vis_cache.begin(); it != vis_cache.end();) {
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.second).count() > 1000) {
                            it = vis_cache.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
            }

            return result;
        }
    }
}
