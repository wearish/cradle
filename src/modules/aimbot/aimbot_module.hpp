#pragma once

#include "../module.hpp"
#include "../../cache/player.hpp"

namespace cradle::modules
{
    class AimbotModule : public Module
    {
    public:
        AimbotModule();
        void on_update() override;
        void on_render() override;
    };
}
