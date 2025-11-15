#include "visualengine.hpp"
#include "../../memory/memory.hpp"

namespace cradle::engine
{

    VisualEngine VisualEngine::get_instance()
    {
        std::uint64_t ve_addr = cradle::memory::read<std::uint64_t>(cradle::memory::baseAddress + Offsets::VisualEngine::Pointer);
        if (!ve_addr || ve_addr < 0x10000)
            return VisualEngine(0);

        return VisualEngine(ve_addr);
    }

    vector2 VisualEngine::get_dimensions()
    {
        if (!address || address < 0x10000)
            return vector2();
        return cradle::memory::read<vector2>(address + Offsets::VisualEngine::Dimensions);
    }

    matrix4 VisualEngine::get_viewmatrix()
    {
        if (!address || address < 0x10000)
            return matrix4();
        return cradle::memory::read<matrix4>(address + Offsets::VisualEngine::ViewMatrix);
    }
}
