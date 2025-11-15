#include "datamodel.hpp"
#include "../../memory/memory.hpp"

namespace cradle::engine
{

    DataModel DataModel::get_instance()
    {
        std::uint64_t visual_engine = cradle::memory::read<std::uint64_t>(cradle::memory::baseAddress + Offsets::VisualEngine::Pointer);
        if (visual_engine && visual_engine > 0x10000)
        {
            std::uint64_t padding = cradle::memory::read<std::uint64_t>(visual_engine + Offsets::VisualEngine::ToDataModel1);
            if (padding && padding > 0x10000)
            {
                std::uint64_t dm_addr = cradle::memory::read<std::uint64_t>(padding + Offsets::VisualEngine::ToDataModel2);
                if (dm_addr && dm_addr > 0x10000)
                {
                    return DataModel(dm_addr);
                }
            }
        }
        return DataModel(0);
    }

    std::uint64_t DataModel::get_place_id()
    {
        if (!address || address < 0x10000)
            return 0;
        return cradle::memory::read<std::uint64_t>(address + Offsets::DataModel::PlaceId);
    }

    std::uint64_t DataModel::get_game_id()
    {
        if (!address || address < 0x10000)
            return 0;
        return cradle::memory::read<std::uint64_t>(address + Offsets::DataModel::GameId);
    }

    bool DataModel::get_game_loaded()
    {
        if (!address || address < 0x10000)
            return false;
        return cradle::memory::read<bool>(address + Offsets::DataModel::GameLoaded);
    }

    std::string DataModel::get_server_ip()
    {
        if (!address || address < 0x10000)
            return "";

        std::uint64_t ip_ptr = cradle::memory::read<std::uint64_t>(address + Offsets::DataModel::ServerIP);
        if (!ip_ptr || ip_ptr < 0x10000)
            return "";

        char buffer[256] = {};
        for (int i = 0; i < 255; i++)
        {
            char c = cradle::memory::read<char>(ip_ptr + i);
            if (c == '\0')
                break;
            buffer[i] = c;
        }
        return std::string(buffer);
    }

    std::uint64_t DataModel::get_place_version()
    {
        if (!address || address < 0x10000)
            return 0;
        return cradle::memory::read<std::uint64_t>(address + Offsets::DataModel::PlaceVersion);
    }

    std::uint64_t DataModel::get_creator_id()
    {
        if (!address || address < 0x10000)
            return 0;
        return cradle::memory::read<std::uint64_t>(address + Offsets::DataModel::CreatorId);
    }

    int DataModel::get_primitive_count()
    {
        if (!address || address < 0x10000)
            return 0;
        return cradle::memory::read<int>(address + Offsets::DataModel::PrimitiveCount);
    }

    Instance DataModel::get_workspace()
    {
        return find_first_child_of_class("Workspace");
    }

    Instance DataModel::get_players()
    {
        return find_first_child_of_class("Players");
    }

    Instance DataModel::get_script_context()
    {
        if (!address || address < 0x10000)
            return Instance(0);

        std::uintptr_t sc_addr = cradle::memory::read<std::uintptr_t>(address + Offsets::DataModel::ScriptContext);
        return Instance(sc_addr);
    }

    Instance DataModel::get_current_camera()
    {
        return get_workspace().find_first_child("Camera");
    }
}
