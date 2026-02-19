#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "game_window.h"
#include "direct3d.h"
#include "shader.h"
#include "shader3d.h"
#include "shader3d_unlit.h"
#include "shader_sprite_effect.h"
#include "shader_post.h"
#include "post_effect.h"
#include "sampler.h"
#include "sprite.h"
#include "Texture.h"
#include "sprite_anim.h"
#include "fade.h"
#include "collision.h"
#include "debug_text.h"
#include <sstream>
#include "system_timer.h"

#include "Audio.h"
#include "key_logger.h"
#include "mouse.h"

#include "scene.h"
#include "cube.h"
#include "meshfield.h"
#include "grid.h"
#include "light.h"
#include "material.h"

#include "circle.h"
#include "debug_imgui.h"

//check using resource
#include <dxgidebug.h>
#include <dxgi1_3.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//DPIスケーリング
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = GameWindow_Create(hInstance);

	SystemTimer_Initialize();
	KeyLogger_Initialize();
	Mouse_Initialize(hWnd);
	//GameWindow_SetMouse(&mouse);
	InitAudio();

	Direct3D_Initialize(hWnd);
	Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Shader3D_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Shader3DUnlit_Initialize();
	ShaderPost_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sampler_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	PostEffect_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	SpriteAnim_Initialize();
	Fade_Initialize();
	Scene_Initialize();
	Cube_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	light_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Material_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Meshfield_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

	Mouse_SetVisible(false);
	//mouse.SetVisible(true);
	
#if defined(DEBUG) || defined(_DEBUG)
	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"resource/texture/consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 0.0f, // offset
		0, 0,
		0.0f, 16.0f);

	Collision_DebugInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Imgui_Initialize(hWnd, Direct3D_GetDevice(), Direct3D_GetContext());

#endif


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// fps,フレーム計測用
	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG frame_count = 0;
	double fps = 0.0;

	/* メッセージ & ゲームループ */
	MSG msg = {};

	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ウィンドウメッセージが来ていたら 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else { // ゲームの処理 

			//fps計測
			current_time = SystemTimer_GetTime(); //システム時刻を取得
			double elapsed_time = current_time - fps_last_time; // fps計測用の経過時間を計算

			if (elapsed_time >= 1.0) // 1秒ごとの計算
			{
				fps = frame_count / elapsed_time;
				fps_last_time = current_time;
				frame_count = 0;
			}

			// 1/60秒ごとに
			elapsed_time = current_time - exec_last_time;
			if (elapsed_time >= (1.0 / 60.0)) {
				//if (true) {
				exec_last_time = current_time;

				//ゲームび更新
				KeyLogger_Update();
				Mouse_Update();

				Scene_Update(elapsed_time);
				Fade_Update(elapsed_time);

				PostEffect_Clear();
				Scene_Draw();

				Direct3D_Clear();
				PostEffect_Begin();
				PostEffect_Draw();
				PostEffect_End();

				Scene_DrawNormal();

				Sprite_Begin();
				Fade_Draw();


#if defined(DEBUG) || defined(_DEBUG)
				if (KeyLogger_IsTrigger(KK_F1)) {
					Imgui_Toggle();
				}
				Imgui_Show();
				std::stringstream ss;
				
				dt.SetText(ss.str().c_str(), {1.0f,1.0f, 0.0f,1.0f});
				
				dt.Draw();
				dt.Clear();
#endif

				Direct3D_Present();
				Scene_Refresh();

				frame_count++;
			}
		}

	} while (msg.message != WM_QUIT);
	
#if defined(DEBUG) || defined(_DEBUG)
	Imgui_Finalize();
	Collision_DebugFinalize();
#endif
	
	Meshfield_Finalize();
	Material_Finalize();
	light_Finalize();
	Cube_Finalize();
	Scene_Finalize();
	Fade_Finalize();
	SpriteAnim_Finalize();
	Sprite_Finalize();
	PostEffect_Finalize();
	Texture_Finalize();
	Sampler_Finalize();
	ShaderPost_Finalize();
	Shader3DUnlit_Finalize();
	Shader3D_Finalize();
	Shader_Finalize();
	Direct3D_Finalize();
	UninitAudio();
	//Mouse_Finalize();
	Mouse_Finalize();
	

	return (int)msg.wParam;
}

