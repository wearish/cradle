#pragma once

#include "../module.hpp"
#include "../../cache/player_cache.hpp"
#include "../../util/engine/visualengine/visualengine.hpp"
#include "../../util/engine/datamodel/datamodel.hpp"
#include "../../util/engine/wallcheck/wallcheck.hpp"
#include "../../util/engine/math.hpp"
#include "../../util/memory/memory.hpp"
#include <imgui.h>

namespace cradle::modules
{
    class ESPModule : public Module
    {
    public:
        ESPModule();
        void on_render() override;
    };
}
