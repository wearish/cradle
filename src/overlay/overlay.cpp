#include "overlay.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <spdlog/spdlog.h>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <tlhelp32.h>
#include <string>
#include "../util/engine/visualengine/visualengine.hpp"
#include "../util/engine/datamodel/datamodel.hpp"
#include "../util/engine/instance/instance.hpp"
#include "../util/engine/wallcheck/wallcheck.hpp"
#include "../cache/player_cache.hpp"
#include "../modules/module_manager.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace cradle
{
    namespace overlay
    {
        UINT Overlay::resize_width = 0;
        UINT Overlay::resize_height = 0;
        Overlay *Overlay::instance = nullptr;
        bool Overlay::menu_visible = false;

        bool Overlay::initialize()
        {
            instance = this;
            createWindow();
            setupDirectX();
            setupImGui();
            return true;
        }

        void Overlay::createWindow()
        {
            ImGui_ImplWin32_EnableDpiAwareness();

            WNDCLASSEX wc = {};
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = windowProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = "BrainRot";
            RegisterClassEx(&wc);

            overlayWindow = CreateWindowEx(
                WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
                "BrainRot",
                "Skibidi",
                WS_POPUP,
                0, 0,
                GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN),
                NULL, NULL, GetModuleHandle(NULL), NULL);

            SetLayeredWindowAttributes(overlayWindow, RGB(0, 0, 0), 255, LWA_COLORKEY);
            ShowWindow(overlayWindow, SW_SHOW);
            UpdateWindow(overlayWindow);
        }

        void Overlay::setupDirectX()
        {
            DXGI_SWAP_CHAIN_DESC scd = {};
            scd.BufferCount = 1;
            scd.BufferDesc.Width = 0;
            scd.BufferDesc.Height = 0;
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            scd.BufferDesc.RefreshRate.Numerator = 60;
            scd.BufferDesc.RefreshRate.Denominator = 1;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scd.OutputWindow = overlayWindow;
            scd.SampleDesc.Count = 1;
            scd.SampleDesc.Quality = 0;
            scd.Windowed = TRUE;
            scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            D3D_FEATURE_LEVEL featureLevel;
            D3D11CreateDeviceAndSwapChain(
                NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED,
                NULL, 0, D3D11_SDK_VERSION,
                &scd, &swapchain, &device, &featureLevel, &context);

            IDXGIDevice *dxgiDevice = nullptr;
            if (SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgiDevice)))
            {
                IDXGIAdapter *dxgiAdapter = nullptr;
                if (SUCCEEDED(dxgiDevice->GetAdapter(&dxgiAdapter)))
                {
                    IDXGIFactory *dxgiFactory = nullptr;
                    if (SUCCEEDED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgiFactory)))
                    {
                        dxgiFactory->MakeWindowAssociation(overlayWindow, DXGI_MWA_NO_ALT_ENTER);
                        dxgiFactory->Release();
                    }
                    dxgiAdapter->Release();
                }
                dxgiDevice->Release();
            }

            ID3D11Texture2D *backBuffer = nullptr;
            if (SUCCEEDED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
            {
                device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
                backBuffer->Release();
            }
        }

        void Overlay::setupImGui()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            ImGuiStyle &style = ImGui::GetStyle();
            style.WindowMinSize = ImVec2(160, 20);
            style.WindowPadding = ImVec2(2, 2);
            style.FramePadding = ImVec2(2, 1);
            style.ItemSpacing = ImVec2(3, 1);
            style.ItemInnerSpacing = ImVec2(2, 2);
            style.Alpha = 0.95f;
            style.WindowRounding = 2.0f;
            style.FrameRounding = 2.0f;
            style.IndentSpacing = 6.0f;
            style.ColumnsMinSpacing = 50.0f;
            style.GrabMinSize = 10.0f;
            style.GrabRounding = 8.0f;
            style.ScrollbarSize = 10.0f;
            style.ScrollbarRounding = 8.0f;

            ImVec4 *colors = style.Colors;
            colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
            colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);

            ImGui_ImplWin32_Init(overlayWindow);
            ImGui_ImplDX11_Init(device, context);
        }

        void Overlay::render()
        {
            static HWND robloxWindow = nullptr;
            static auto lastRobloxCheck = std::chrono::steady_clock::now();
            auto currentTime = std::chrono::steady_clock::now();

            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastRobloxCheck).count() >= 5)
            {
                lastRobloxCheck = currentTime;

                HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                bool robloxRunning = false;

                if (snapshot != INVALID_HANDLE_VALUE)
                {
                    PROCESSENTRY32 pe32;
                    pe32.dwSize = sizeof(PROCESSENTRY32);

                    if (Process32First(snapshot, &pe32))
                    {
                        do
                        {
                            if (strcmp(pe32.szExeFile, "RobloxPlayerBeta.exe") == 0)
                            {
                                robloxRunning = true;
                                break;
                            }
                        } while (Process32Next(snapshot, &pe32));
                    }
                    CloseHandle(snapshot);
                }

                if (!robloxRunning)
                {
                    robloxWindow = nullptr;
                }
                else
                {
                    robloxWindow = FindWindowA(nullptr, "Roblox");
                }
            }

            if (!robloxWindow)
            {
                robloxWindow = FindWindowA(nullptr, "Roblox");
            }

            if (robloxWindow)
            {
                RECT clientRect;
                GetClientRect(robloxWindow, &clientRect);

                POINT topLeft = {0, 0};
                ClientToScreen(robloxWindow, &topLeft);

                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;

                MoveWindow(overlayWindow, topLeft.x, topLeft.y, width, height, TRUE);
            }

            if (resize_width != 0 && resize_height != 0)
            {
                if (renderTargetView)
                {
                    renderTargetView->Release();
                    renderTargetView = nullptr;
                }

                swapchain->ResizeBuffers(0, resize_width, resize_height, DXGI_FORMAT_UNKNOWN, 0);
                resize_width = resize_height = 0;

                ID3D11Texture2D *backBuffer = nullptr;
                if (SUCCEEDED(swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
                {
                    device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
                    backBuffer->Release();
                }
            }

            if (GetAsyncKeyState(VK_INSERT) & 1)
            {
                showMenu = !showMenu;
                menu_visible = showMenu;

                LONG_PTR exStyle = GetWindowLongPtr(overlayWindow, GWL_EXSTYLE);
                if (showMenu)
                {
                    exStyle &= ~WS_EX_TRANSPARENT;
                }
                else
                {
                    exStyle |= WS_EX_TRANSPARENT;
                }
                SetWindowLongPtr(overlayWindow, GWL_EXSTYLE, exStyle);
                SetWindowPos(overlayWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }

            static bool keys_pressed[256] = {};
            for (int i = 0; i < 256; i++)
            {
                bool is_down = (GetAsyncKeyState(i) & 0x8000) != 0;

                if (is_down && !keys_pressed[i])
                {
                    modules::ModuleManager::get_instance().on_key_press(i);
                }
                else if (!is_down && keys_pressed[i])
                {
                    modules::ModuleManager::get_instance().on_key_release(i);
                }

                keys_pressed[i] = is_down;
            }

            HWND foreground = GetForegroundWindow();
            bool roblox_active = robloxWindow && (foreground == robloxWindow || foreground == overlayWindow);

            if (roblox_active)
            {
                cradle::engine::PlayerCache::update_cache();

                cradle::engine::DataModel dm = cradle::engine::DataModel::get_instance();
                if (dm.is_valid())
                {
                    cradle::engine::Wallcheck::update_world_cache(dm);
                }

                modules::ModuleManager::get_instance().update_all();
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            if (roblox_active)
            {
                modules::ModuleManager::get_instance().render_all();
            }

            if (showMenu && roblox_active)
            {
                static auto get_key_name = [](int vk_code) -> const char *
                {
                    static char key_name[32];
                    if (vk_code == 0) return "none";
                    switch (vk_code) {
                        case VK_LBUTTON: return "lmb";
                        case VK_RBUTTON: return "rmb";
                        case VK_MBUTTON: return "scroll";
                        case VK_XBUTTON1: return "x1";
                        case VK_XBUTTON2: return "x2";
                        case VK_SHIFT: return "shift";
                        case VK_CONTROL: return "ctrl";
                        case VK_MENU: return "alt";
                        default: snprintf(key_name, sizeof(key_name), "%c", vk_code); return key_name;
                    }
                };

                ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

                if (ImGui::Begin("cradle", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
                {
                    if (!selected_module)
                    {
                        auto mods = modules::ModuleManager::get_instance().get_all_modules();
                        if (!mods.empty())
                            selected_module = mods.front();
                    }

                    ImGui::Columns(2, nullptr, true);
                    ImGui::SetColumnWidth(0, 110);

                    ImGui::BeginChild("ModuleList", ImVec2(0, 0), false);
                    for (auto *mod : modules::ModuleManager::get_instance().get_all_modules())
                    {
                        bool is_selected = selected_module == mod;
                        ImGui::PushStyleColor(ImGuiCol_Text, mod->is_enabled() ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        if (ImGui::Selectable(mod->get_name().c_str(), is_selected, 0))
                            selected_module = mod;
                        ImGui::PopStyleColor();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("%s", mod->get_description().c_str());
                    }
                    ImGui::EndChild();

                    ImGui::NextColumn();

                    ImGui::BeginChild("ModuleSettings", ImVec2(0, 0), false);
                    ImGui::TextColored(ImVec4(0.92f, 0.18f, 0.29f, 1.0f), "%s", selected_module->get_name().c_str());
                    ImGui::TextDisabled("%s", selected_module->get_description().c_str());
                    
                    bool enabled = selected_module->is_enabled();
                    if (ImGui::Checkbox("enabled", &enabled))
                        selected_module->set_enabled(enabled);
                    
                    ImGui::Separator();
                    
                    static int delay_counter = 0;
                    static bool waiting_for_key = false;
                    
                    int current_key = selected_module->get_keybind();
                    const char *modes[] = {"toggle", "hold"};
                    int current_mode = (int)selected_module->get_keybind_mode();
                    
                    ImGui::Text("keybind: ");
                    ImGui::SameLine();
                    
                    if (!waiting_for_key)
                    {
                        if (ImGui::Button(get_key_name(current_key), ImVec2(100, 0)))
                        {
                            waiting_for_key = true;
                            delay_counter = 0;
                        }
                        
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                        {
                            ImGui::OpenPopup("KeybindPopup");
                        }
                    }
                    else
                    {
                        ImGui::Button("...", ImVec2(100, 0));
                        if (++delay_counter > 3)
                        {
                            for (int i = 0; i < 256; i++)
                            {
                                if (i == VK_INSERT || i == VK_LBUTTON) continue;
                                
                                if (i == VK_ESCAPE || i == VK_BACK)
                                {
                                    if (GetAsyncKeyState(i) & 0x8000)
                                    {
                                        selected_module->set_keybind(0);
                                        modules::ModuleManager::get_instance().update_keybinds();
                                        waiting_for_key = false;
                                        delay_counter = 0;
                                        break;
                                    }
                                }
                                else if (GetAsyncKeyState(i) & 0x8000)
                                {
                                    selected_module->set_keybind(i);
                                    modules::ModuleManager::get_instance().update_keybinds();
                                    waiting_for_key = false;
                                    delay_counter = 0;
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (ImGui::BeginPopup("KeybindPopup"))
                    {
                        ImGui::Text("mode");
                        ImGui::Separator();
                        for (int i = 0; i < 2; i++)
                        {
                            if (ImGui::MenuItem(modes[i], nullptr, i == current_mode))
                            {
                                selected_module->set_keybind_mode((modules::KeybindMode)i);
                            }
                        }
                        ImGui::EndPopup();
                    }
                    
                    auto &settings = selected_module->get_settings();
                    if (!settings.empty())
                    {
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.47f, 0.77f, 0.83f, 1.0f), "settings");
                        ImGui::Spacing();

                        for (auto &s : settings)
                        {
                            switch (s.type)
                            {
                            case modules::SettingType::BOOL:
                                ImGui::Checkbox(s.name.c_str(), &s.value.bool_val);
                                break;
                            case modules::SettingType::INT:
                                ImGui::SliderInt(s.name.c_str(), &s.value.int_val, s.range.int_range.min, s.range.int_range.max);
                                break;
                            case modules::SettingType::FLOAT:
                                ImGui::SliderFloat(s.name.c_str(), &s.value.float_val, s.range.float_range.min, s.range.float_range.max);
                                break;
                            case modules::SettingType::COLOR:
                                ImGui::ColorEdit4(s.name.c_str(), s.value.color_val);
                                break;
                            }
                        }
                    }

                    ImGui::EndChild();

                    ImGui::Columns(1);
                    ImGui::End();
                }
            }

            ImGui::Render();

            float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            context->OMSetRenderTargets(1, &renderTargetView, NULL);
            context->ClearRenderTargetView(renderTargetView, clearColor);

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            swapchain->Present(1, 0);
        }

        bool Overlay::isRunning()
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    return false;
            }
            return true;
        }

        LRESULT CALLBACK Overlay::windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
        {
            if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
                return true;

            switch (msg)
            {
            case WM_SIZE:
                if (wparam == SIZE_MINIMIZED)
                    return 0;
                resize_width = (UINT)LOWORD(lparam);
                resize_height = (UINT)HIWORD(lparam);
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            }

            return DefWindowProc(hwnd, msg, wparam, lparam);
        }

        void Overlay::cleanup()
        {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            if (renderTargetView)
                renderTargetView->Release();
            if (swapchain)
                swapchain->Release();
            if (context)
                context->Release();
            if (device)
                device->Release();
            if (overlayWindow)
                DestroyWindow(overlayWindow);
        }

    }
}
