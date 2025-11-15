#include "instance.hpp"

namespace cradle::engine
{

    std::string Instance::get_name()
    {
        if (!address || address < 0x10000)
            return "null";

        std::uint64_t name_ptr = cradle::memory::read<std::uint64_t>(address + Offsets::Instance::Name);
        if (!name_ptr || name_ptr < 0x10000)
            return "unknown";

        char buffer[256] = {};
        for (int i = 0; i < 255; i++)
        {
            char c = cradle::memory::read<char>(name_ptr + i);
            if (c == '\0')
                break;
            buffer[i] = c;
        }
        return std::string(buffer);
    }

    std::string Instance::get_class_name() const
    {
        if (!address || address < 0x10000)
            return "null";

        std::uint64_t class_descriptor = cradle::memory::read<std::uint64_t>(address + Offsets::Instance::ClassDescriptor);
        if (!class_descriptor || class_descriptor < 0x10000)
            return "unknown";

        std::uint64_t class_name_ptr = cradle::memory::read<std::uint64_t>(class_descriptor + Offsets::Instance::ClassName);
        if (!class_name_ptr || class_name_ptr < 0x10000)
            return "unknown";

        char buffer[256] = {};
        for (int i = 0; i < 255; i++)
        {
            char c = cradle::memory::read<char>(class_name_ptr + i);
            if (c == '\0')
                break;
            buffer[i] = c;
        }
        return std::string(buffer);
    }

    std::vector<Instance> Instance::get_children()
    {
        std::vector<Instance> children;
        if (!address || address < 0x10000)
            return children;

        std::uint64_t children_start = cradle::memory::read<std::uint64_t>(address + Offsets::Instance::ChildrenStart);
        if (!children_start || children_start < 0x10000)
            return children;

        std::uint64_t children_end = cradle::memory::read<std::uint64_t>(children_start + Offsets::Instance::ChildrenEnd);

        for (std::uint64_t i = cradle::memory::read<std::uint64_t>(children_start);
             i != children_end;
             i += 0x10)
        {
            std::uint64_t child_addr = cradle::memory::read<std::uint64_t>(i);
            if (child_addr && child_addr > 0x10000)
            {
                children.emplace_back(child_addr);
            }
        }

        return children;
    }

    Instance Instance::find_first_child(const std::string &name)
    {
        std::vector<Instance> children = get_children();
        for (auto &child : children)
        {
            if (child.get_name() == name)
            {
                return child;
            }
        }
        return Instance(0);
    }

    Instance Instance::find_first_child_of_class(const std::string &class_name)
    {
        std::vector<Instance> children = get_children();
        for (auto &child : children)
        {
            if (child.get_class_name() == class_name)
            {
                return child;
            }
        }
        return Instance(0);
    }

    std::vector<Instance> Instance::find_descendants_of_class(const std::string &class_name)
    {
        std::vector<Instance> result;
        std::vector<Instance> stack = get_children();

        while (!stack.empty())
        {
            Instance current = stack.back();
            stack.pop_back();

            if (current.get_class_name() == class_name)
            {
                result.push_back(current);
            }

            auto children = current.get_children();
            stack.insert(stack.end(), children.begin(), children.end());
        }

        return result;
    }

    Instance Instance::get_parent()
    {
        if (!address || address < 0x10000)
            return Instance(0);
        std::uint64_t parent_addr = cradle::memory::read<std::uint64_t>(address + Offsets::Instance::Parent);
        return Instance(parent_addr);
    }

    bool Instance::is_descendant_of(const Instance &ancestor) const
    {
        if (!ancestor.is_valid() || !address)
            return false;

        Instance current = *this;
        while (current.address != 0)
        {
            if (current.address == ancestor.address)
            {
                return true;
            }
            current = current.get_parent();
        }
        return false;
    }

    vector3 Instance::get_pos() const
    {
        if (!is_valid())
            return vector3(0, 0, 0);

        // camera instances store position directly at their camera offset
        std::string cls = get_class_name();
        if (cls == "Camera") {
            return cradle::memory::read<vector3>(address + Offsets::Camera::Position);
        }

        std::uint64_t prim = cradle::memory::read<std::uint64_t>(address + Offsets::BasePart::Primitive);
        if (!prim || prim <= 0x10000)
            return vector3(0, 0, 0);
        return cradle::memory::read<vector3>(prim + Offsets::BasePart::Position);
    }

    cframe Instance::get_cframe() const
    {
        if (!is_valid())
            return cframe();

        std::string cls = get_class_name();
        if (cls == "Camera") {
            matrix3 rot = cradle::memory::read<matrix3>(address + Offsets::Camera::Rotation);
            vector3 pos = cradle::memory::read<vector3>(address + Offsets::Camera::Position);
            return cframe(rot, pos);
        }

        std::uint64_t prim = cradle::memory::read<std::uint64_t>(address + Offsets::BasePart::Primitive);
        if (!prim || prim <= 0x10000)
            return cframe();
        return cradle::memory::read<cframe>(prim + Offsets::BasePart::Rotation);
    }

    Instance Instance::get_character() const
    {
        if (!is_valid())
            return Instance(0);
        std::uint64_t char_addr = cradle::memory::read<std::uint64_t>(address + Offsets::Player::ModelInstance);
        return Instance(char_addr);
    }

    Instance Instance::get_local_player() const
    {
        if (!is_valid())
            return Instance(0);
        std::uint64_t local_addr = cradle::memory::read<std::uint64_t>(address + Offsets::Player::LocalPlayer);
        return Instance(local_addr);
    }

    float Instance::get_health() const
    {
        if (!is_valid())
            return 0.0f;
        return cradle::memory::read<float>(address + Offsets::Humanoid::Health);
    }

    float Instance::get_max_health() const
    {
        if (!is_valid())
            return 100.0f;
        return cradle::memory::read<float>(address + Offsets::Humanoid::MaxHealth);
    }

    int Instance::get_rig_type() const
    {
        if (!is_valid())
            return 0;
        return cradle::memory::read<int>(address + Offsets::Humanoid::RigType);
    }
}
