#pragma once
#include <cstdint>
#include "../instance/instance.hpp"
#include "../math.hpp"
#include "../offsets.hpp"

namespace cradle::engine
{
    class VisualEngine
    {
    public:
        std::uint64_t address = 0;

        VisualEngine() = default;
        VisualEngine(std::uint64_t addr) : address(addr) {}

        static VisualEngine get_instance();

        vector2 get_dimensions();
        matrix4 get_viewmatrix();

        bool is_valid() const { return address > 0x10000; }
    };
}
