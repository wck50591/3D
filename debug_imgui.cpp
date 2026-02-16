#include "debug_imgui.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include "Key_Logger.h"
#include "keyboard.h"
#include <DirectXMath.h>
#include "camera.h"
#include "direct3d.h"
#include "cube.h"
#include "meshfield.h"
#include "sampler.h"
#include "light.h"

using namespace DirectX;

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
//static IDXGISwapChain* g_pSwapChain = nullptr;
//static bool                     g_SwapChainOccluded = false;
//static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
//static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

static bool show_toggle = false;
static bool show_demo_window = true;
static bool show_main_window = true;
static bool show_another_window = false;
static int fillMode = 0;
static bool cube_show = false;
static bool cube_clear = false;
static int filter = 0;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void Imgui_Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    show_toggle = false;

    g_pd3dDevice = pDevice;
    g_pd3dDeviceContext = pContext;

    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

void Imgui_Finalize()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Imgui_Update()
{
}

void Imgui_Show()
{
    if (show_toggle == false) {
        return;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    //Camera
    XMFLOAT3 position = Camera_GetPosition();
    float p_x = position.x;
    float p_y = position.y;
    float p_z = position.z;
    float fov = XMConvertToDegrees(Camera_GetFov());

    //Light
    /*XMFLOAT4 l_directional= light_GetDirectional();
    float l_d_x = l_directional.x;
    float l_d_y = l_directional.y;
    float l_d_z = l_directional.z;
    float l_d_w = l_directional.w;

    XMFLOAT4 l_color = light_GetColor();
    float l_c_r = l_color.x;
    float l_c_g = l_color.y;
    float l_c_b = l_color.z;
    float l_c_a = l_color.w;*/

    //Object
    float mesh_p_x = Meshfield_GetPositionX();
    float mesh_p_z = Meshfield_GetPositionZ();

    static int counter = 0;
    if (show_main_window)
    {
        ImGui::Begin("Hello, world!", &show_main_window);

        static float o_x = 0.0f;
        static float o_y = 0.0f;
        static float o_z = 0.0f;

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MainTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Camera"))
            {
                ImGui::SeparatorText("Position");
                ImGui::Text("X");
                ImGui::SameLine();
                ImGui::SliderFloat("X", &p_x, -100.0f, 100.0f);
                ImGui::Text("Y");
                ImGui::SameLine();
                ImGui::SliderFloat("Y", &p_y, -100.0f, 100.0f);
                ImGui::Text("Z");
                ImGui::SameLine();
                ImGui::SliderFloat("Z", &p_z, -100.0f, 100.0f);

                ImGui::SeparatorText("Front");
                ImGui::Text("X %.2f", Camera_GetFront().x);
                ImGui::SameLine();
                ImGui::Text("Y %.2f", Camera_GetFront().y);
                ImGui::SameLine();
                ImGui::Text("Z %.2f", Camera_GetFront().z);

                ImGui::SeparatorText("Up");
                ImGui::Text("X %.2f", Camera_GetUp().x);
                ImGui::SameLine();
                ImGui::Text("Y %.2f", Camera_GetUp().y);
                ImGui::SameLine();
                ImGui::Text("Z %.2f", Camera_GetUp().z);

                ImGui::SeparatorText("Right");
                ImGui::Text("X %.2f", Camera_GetRight().x);
                ImGui::SameLine();
                ImGui::Text("Y %.2f", Camera_GetRight().y);
                ImGui::SameLine();
                ImGui::Text("Z %.2f", Camera_GetRight().z);

                ImGui::SeparatorText("Fov");
                ImGui::SliderFloat("Fov", &fov, 0.1f, 200.0f);

                Camera_SetPositionX(p_x);
                Camera_SetPositionY(p_y);
                Camera_SetPositionZ(p_z);
                Camera_SetFov(fov);

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Light"))
            {

                /*ImGui::SeparatorText("Directional");

                ImGui::Text("X");
                ImGui::SameLine();
                ImGui::SliderFloat("X", &l_d_x, -1.0f, 1.0f);
                ImGui::Text("Y");
                ImGui::SameLine();
                ImGui::SliderFloat("Y", &l_d_y, -1.0f, 1.0f);
                ImGui::Text("Z");
                ImGui::SameLine();
                ImGui::SliderFloat("Z", &l_d_z, -1.0f, 1.0f);
                ImGui::Text("W");
                ImGui::SameLine();
                ImGui::SliderFloat("W", &l_d_w, -1.0f, 1.0f);

                light_SetDirectional(XMFLOAT4(l_d_x, l_d_y, l_d_z, l_d_w));


                ImGui::SeparatorText("Color");

                XMFLOAT4 color = {l_c_r, l_c_g, l_c_b, l_c_a};
                ImGui::ColorEdit4("Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float);
                light_SetColor(color);*/

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Object"))
            {
                ImGui::SeparatorText("Sampler Filter");
                ImGui::RadioButton("Point", &filter, 0);
                ImGui::RadioButton("Linear", &filter, 1);
                ImGui::RadioButton("Anisotropic", &filter, 2);

                if (ImGui::BeginTabBar("ObjectTabBar", tab_bar_flags))
                {
                    if (ImGui::BeginTabItem("Cube"))
                    {
                        ImGui::SeparatorText("Spawn");
                        ImGui::DragFloat("X", &o_x, 0.1f);
                        ImGui::DragFloat("Y", &o_y, 0.1f);
                        ImGui::DragFloat("Z", &o_z, 0.1f);

                        if (ImGui::Button("Cube")) {
                            cube_show = true;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Clear")) {
                            cube_clear = true;
                        }
                        ImGui::EndTabItem();
                    }
                   

                    if (ImGui::BeginTabItem("Mesh Field"))
                    {
                        ImGui::Text("Mesh Position X");
                        ImGui::DragFloat("Mesh Position X", &mesh_p_x, 0.1f);
                        ImGui::Text("Mesh Position Z");
                        ImGui::DragFloat("Mesh Postiion Z", &mesh_p_z, 0.1f);

                        Meshfield_SetPositionX(mesh_p_x);
                        Meshfield_SetPositionZ(mesh_p_z);

                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Game"))
            {

                ImGui::SeparatorText("Fill Mode");
                ImGui::RadioButton("Solid", &fillMode, 0);
                ImGui::RadioButton("Wireframe", &fillMode, 1);

                if (fillMode == 0) {
                    Direct3D_SetRasterizer(D3D11_FILL_SOLID);
                }
                if (fillMode == 1) {
                    Direct3D_SetRasterizer(D3D11_FILL_WIREFRAME);
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        if (cube_show) {
            if (filter == 0) {
                Sampler_SetFilterPoint();
            }
            if (filter == 1) {
                Sampler_SetFilterLinear();
            }
            if (filter == 2) {
                Sampler_SetFilterAnisotropic();
            }

            XMMATRIX mtxTrans = XMMatrixTranslation(o_x, o_y, o_z);
            XMMATRIX mtxWorld = mtxTrans;
            //Cube_Draw(mtxWorld);

        }
       
        if (cube_clear) {
            cube_show = false;
            cube_clear = false;
        }

        //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);

      
        //ImGui::SliderFloat("Camera Position Y ", , -1.0f, 1.0f);
        //ImGui::SliderFloat("Camera Position Z ", , -1.0f, 1.0f);
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //    counter++;
        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Imgui_Toggle()
{
    if (show_toggle) {
        show_toggle = false;
    }
    else {
        show_toggle = true;
        show_main_window = true;
    }
}
