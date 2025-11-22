#include <spdlog/spdlog.h>
#include "overlay/overlay.hpp"
#include "util/memory/memory.hpp"
#include "util/engine/offsets.hpp"
#include "util/engine/datamodel/datamodel.hpp"
#include "cache/player_cache.hpp"
#include "modules/module_manager.hpp"
#include "modules/esp/esp_module.hpp"
#include "modules/aimbot/aimbot_module.hpp"
#include "modules/triggerbot/triggerbot_module.hpp"

int main()
{
    cradle::memory::processPid = cradle::memory::FindProcess("RobloxPlayerBeta.exe");

    if (cradle::memory::processPid == 0)
    {
        spdlog::error("failed to find robloxplayerbeta.exe");
        return 1;
    }

    cradle::memory::EnsureSyscallInit();
    cradle::memory::baseAddress = cradle::memory::GetProcessBase();
    spdlog::info("attached to roblox (pid: {}, base: 0x{:X})", cradle::memory::processPid, cradle::memory::baseAddress);

    cradle::overlay::Overlay overlay;
    if (!overlay.initialize())
    {
        spdlog::error("overlay initialization failed");
        return 1;
    }

    auto &module_manager = cradle::modules::ModuleManager::get_instance();
    auto esp_module = std::make_unique<cradle::modules::ESPModule>();
    auto aimbot_module = std::make_unique<cradle::modules::AimbotModule>();
    auto triggerbot_module = std::make_unique<cradle::modules::TriggerbotModule>();
    module_manager.register_module(std::move(esp_module));
    module_manager.register_module(std::move(aimbot_module));
    module_manager.register_module(std::move(triggerbot_module));
    spdlog::info("module manager initialized");

    spdlog::info("overlay initialized - press insert to toggle menu");

    while (overlay.isRunning())
    {
        overlay.render();
    }

    overlay.cleanup();
    return 0;
}