#pragma once
#include "../module.hpp"

namespace cradle::modules
{
    class TriggerbotModule : public Module
    {
    public:
        TriggerbotModule();
        void on_update() override;
    };
}
