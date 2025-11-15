#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "../modules/module.hpp"

namespace cradle
{
    namespace overlay
    {
        class Overlay
        {
        private:
            ID3D11Device *device = nullptr;
            ID3D11DeviceContext *context = nullptr;
            IDXGISwapChain *swapchain = nullptr;
            ID3D11RenderTargetView *renderTargetView = nullptr;
            HWND overlayWindow = nullptr;
            bool showMenu = false;
            modules::Module* selected_module = nullptr;
            int selected_tab = 0;
            
            static UINT resize_width;
            static UINT resize_height;
            static Overlay* instance;
            static bool menu_visible;

        public:
            bool initialize();
            void cleanup();
            void render();
            bool isRunning();
            static bool is_menu_open() { return menu_visible; }

            static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        private:
            void createWindow();
            void setupDirectX();
            void setupImGui();
            void updateWindowPosition();
            // UI methods removed as requested
        };

        
    }
}
