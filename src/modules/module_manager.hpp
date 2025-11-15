#pragma once

#include "module.hpp"
#include <map>
#include <memory>
#include <memory>

namespace cradle::modules
{
    class ModuleManager
    {
    private:
        std::vector<std::unique_ptr<Module>> modules;
        std::map<int, Module *> keybinds;
        std::map<int, bool> key_states;

        ModuleManager() = default;

    public:
        static ModuleManager &get_instance()
        {
            static ModuleManager instance;
            return instance;
        }

        ModuleManager(const ModuleManager &) = delete;
        ModuleManager &operator=(const ModuleManager &) = delete;

        void register_module(std::unique_ptr<Module> module)
        {
            modules.push_back(std::move(module));
        }

        void update_keybinds()
        {
            keybinds.clear();
            for (auto &m : modules)
            {
                if (m->get_keybind() != 0)
                {
                    keybinds[m->get_keybind()] = m.get();
                }
            }
        }

        void on_key_press(int key)
        {
            if (key == 0)
                return;
            auto it = keybinds.find(key);
            if (it != keybinds.end())
            {
                Module *mod = it->second;
                if (mod->get_keybind_mode() == KeybindMode::TOGGLE)
                {
                    mod->toggle();
                }
                else
                {
                    mod->set_enabled(true);
                }
                key_states[key] = true;
            }
        }

        void on_key_release(int key)
        {
            if (key == 0)
                return;
            auto it = keybinds.find(key);
            if (it != keybinds.end())
            {
                Module *mod = it->second;
                if (mod->get_keybind_mode() == KeybindMode::HOLD)
                {
                    mod->set_enabled(false);
                }
                key_states[key] = false;
            }
        }

        bool is_key_down(int key)
        {
            auto it = key_states.find(key);
            return it != key_states.end() && it->second;
        }

        void update_all()
        {
            for (auto &m : modules)
            {
                if (m->is_enabled()) {
                    m->on_update();
                }
            }
        }

        void render_all()
        {
            for (auto &m : modules)
            {
                if (m->is_enabled()) {
                    m->on_render();
                }
            }
        }

        std::vector<Module *> get_all_modules()
        {
            std::vector<Module *> result;
            for (auto &m : modules)
                result.push_back(m.get());
            return result;
        }
    };
}
