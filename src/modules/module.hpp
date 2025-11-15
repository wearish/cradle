#pragma once

#include <string>
#include <vector>

namespace cradle::modules
{
    enum class SettingType { BOOL, INT, FLOAT, COLOR };
    enum class KeybindMode { TOGGLE, HOLD };

    struct Setting
    {
        std::string name;
        SettingType type;
        
        union {
            bool bool_val;
            int int_val;
            float float_val;
            float color_val[4];
        } value;
        
        union {
            struct { int min; int max; } int_range;
            struct { float min; float max; } float_range;
        } range;

        Setting(const std::string& n, bool val) : name(n), type(SettingType::BOOL) { value.bool_val = val; }
        Setting(const std::string& n, int val, int mn, int mx) : name(n), type(SettingType::INT) {
            value.int_val = val;
            range.int_range = {mn, mx};
        }
        Setting(const std::string& n, float val, float mn, float mx) : name(n), type(SettingType::FLOAT) {
            value.float_val = val;
            range.float_range = {mn, mx};
        }
        Setting(const std::string& n, float r, float g, float b, float a) : name(n), type(SettingType::COLOR) {
            value.color_val[0] = r;
            value.color_val[1] = g;
            value.color_val[2] = b;
            value.color_val[3] = a;
        }
    };

    class Module
    {
    protected:
        std::string name;
        std::string desc;
        int keybind = 0;
        KeybindMode keybind_mode = KeybindMode::TOGGLE;
        bool enabled = false;
        std::vector<Setting> settings;

    public:
        Module(const std::string& n, const std::string& d) : name(n), desc(d) {}
        virtual ~Module() = default;

        virtual void on_enable() {}
        virtual void on_disable() {}
        virtual void on_update() {}
        virtual void on_render() {}

        void toggle() {
            enabled = !enabled;
            enabled ? on_enable() : on_disable();
        }

        void set_enabled(bool state) {
            if (enabled != state) {
                enabled = state;
                enabled ? on_enable() : on_disable();
            }
        }

        const std::string& get_name() const { return name; }
        const std::string& get_description() const { return desc; }
        int get_keybind() const { return keybind; }
        void set_keybind(int k) { keybind = k; }
        KeybindMode get_keybind_mode() const { return keybind_mode; }
        void set_keybind_mode(KeybindMode mode) { keybind_mode = mode; }
        bool is_enabled() const { return enabled; }
        std::vector<Setting>& get_settings() { return settings; }

        Setting* get_setting(const std::string& setting_name) {
            for (auto& s : settings) {
                if (s.name == setting_name) return &s;
            }
            return nullptr;
        }
    };
}
