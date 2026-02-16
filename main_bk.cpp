#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "game_window.h"
#include "direct3d.h"
#include "shader.h"
#include "sprite.h"
#include "Texture.h"
#include "sprite_anim.h"
#include "debug_text.h"
#include "debug_ostream.h"
#include <sstream>
#include "system_timer.h"
#include "Math.h"
#include "polygon.h"

#include <DirectXMath.h>
using namespace DirectX;

//#include <sstream>
//#include "debug_ostream.h"

//constexpr char FILE_NAME[] = "tekito.png";

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	HWND hWnd = GameWindow_Create(hInstance);

	SystemTimer_Initialize();
	Direct3D_Initialize(hWnd);
	Shader_Initialize(Direct3D_GetDevice(),Direct3D_GetContext());
	Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	SpriteAnim_Initialize();

	// fps,フレーム計測用
	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG frame_count = 0;
	double fps = 0.0;

	/* メッセージ & ゲームループ */
	MSG msg = {};

	/*float x = 0.0f;
	float y = 0.0f;
	float w = 512.0f;
	float h = 512.0f;*/

	Polygon_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(), 
		L"consolab_ascii_512.png", 
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 0.0f, // offset
		0, 0,
		0.0f, 0.0f);



	int texid_fox = Texture_Load(L"fox.png", TRUE);
	int texid_fox2 = Texture_Load(L"fox_01.png", TRUE);
	int texid_coco = Texture_Load(L"kokosozai.png",TRUE);
	int texid_run01 = Texture_Load(L"runningman001.png", TRUE);
	int texid_run01_right = Texture_Load(L"runningman001_right.png", TRUE);
	int texid_white_dot = Texture_Load(L"white_dot.png", TRUE);

	int aid_rw = SpriteAnim_RegisterPattern(texid_coco, 13, 16, 0.1, { 32,32 }, { 0, 0 });
	int aid_lw = SpriteAnim_RegisterPattern(texid_coco, 13, 16, 0.1, { 32,32 }, { 0 , 32 });
	int aid_sm = SpriteAnim_RegisterPattern(texid_coco, 6, 16, 0.1,  { 32,32 }, { 0 , 32 * 2 });
	int aid_to = SpriteAnim_RegisterPattern(texid_coco, 4, 16, 0.1, { 32,32 }, { 32 * 2, 32 * 5 }, false);
	int aid_sh = SpriteAnim_RegisterPattern(texid_coco, 15, 16, 0.1, { 32,32 }, { 0, 32 * 4 });
	int aid_run = SpriteAnim_RegisterPattern(texid_run01, 10, 5, 0.1, { 140, 200 }, { 0, 0 });
	int aid_run_r = SpriteAnim_RegisterPattern(texid_run01_right, 10, 5, 0.1, { 140, 200 }, { 0, 0 });

	int pid_01 = SpriteAnim_CreatePlayer(aid_rw);
	int pid_02 = SpriteAnim_CreatePlayer(aid_sm);
	int pid_03 = SpriteAnim_CreatePlayer(aid_to);
	int pid_04 = SpriteAnim_CreatePlayer(aid_lw);
	int pid_05 = SpriteAnim_CreatePlayer(aid_sh);
	int pid_run= SpriteAnim_CreatePlayer(aid_run);
	int pid_run_r = SpriteAnim_CreatePlayer(aid_run_r);
	

	float angle = 0.0f;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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
				SpriteAnim_Update(elapsed_time);

				//ゲームの絵画
				Direct3D_Clear();

				Sprite_Begin();
				
				Sprite_Draw(texid_fox, 0.0f, 0.0f);
				Sprite_Draw(texid_fox2, 100.0f, 100.0f, 256.0f, 256.0f);

				SpriteAnim_Draw(pid_01, 100.0f, 100.0f, 256.0f, 256.0f);
				SpriteAnim_Draw(pid_02, 400.0f, 100.0f, 256.0f, 256.0f);
				SpriteAnim_Draw(pid_03, 800.0f, 100.0f, 256.0f, 256.0f);
				SpriteAnim_Draw(pid_04, 600.0f, 100.0f, 256.0f, 256.0f);
				SpriteAnim_Draw(pid_05, 1000.0f, 100.0f, 256.0f, 256.0f);
				//SpriteAnim_Draw(pid_06, 100.0f, 400.0f, 280.0f, 400.0f);

				angle += 1 * elapsed_time;
				Sprite_Draw(texid_coco, 256.0f, 256.0f, 256.0f, 256.0f, 0, 0, 32, 51, angle);

				//float ky = sinf(angle) * 100.0f + 64.0f;
				//angle += 5.0 * elapsed_time;

				//hal::dout << "angle : " << angle << std::endl;

				//Sprite_Draw(texid_fox2, 500.0f, ky, 256.0f, 256.0f);

				//Texture_SetTexture(texid_white_dot);

				//Polygon_Draw();

	#if defined(DEBUG) || defined(_DEBUG)

				std::stringstream ss;
				ss << "fps : " << fps << std::endl;
				dt.SetText(ss.str().c_str());

				//dt.SetText("Hello World! \tovo\n");
				//dt.SetText("RED!", { 1.0f, 0.0f, 0.0f, 1.0f });
				//dt.SetText("GREEN!", { 0.0f, 1.0f, 0.0f, 1.0f });
				//dt.SetText("BLUE!\n", { 0.0f, 0.0f, 1.0f, 1.0f });
				//dt.SetText("ABCDEFGHIJKLMNOPGRSTUVWXYZ\n", { 0.0f, 0.0f, 0.0f, 1.0f });
				//dt.SetText("transparent!\n", { 1.0f, 1.0f, 1.0f, 0.3f });

				dt.Draw();
				dt.Clear();
	#endif

				Direct3D_Present();

				frame_count++;
			}
		}
	} while (msg.message != WM_QUIT);

	//メッセージループ
	// 
	// MSG msg;
	// 
	// float x = 0.0f;
	// 
	//while (GetMessage(&msg, nullptr, 0, 0)) {
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);

	//	Direct3D_Clear();
	// 
	//	Sprite_Draw(x, 32.0f);
	//	x += 0.3f;

	//	Direct3D_Present();
	//}
	Polygon_Finalize();
	SpriteAnim_Finalize();
	Sprite_Finalize();
	Texture_Finalize();
	Shader_Finalize();
	Direct3D_Finalize();

	return (int)msg.wParam;
	/*std::stringstream ss;
	ss << "テクスチャファイル：" << FILE_NAME << "が読み込めませんでした";

	int a = MessageBox(nullptr, ss.str().c_str(), "キャプション", MB_YESNOCANCEL
	| MB_ICONERROR | MB_DEFBUTTON2);

	if (a == IDYES) {
		MessageBox(nullptr,"YES","YES",MB_OK);
	}
	else if (a == IDNO) {
		MessageBox(nullptr, "NO", "NO", MB_OK);
	}
	else if (a == IDCANCEL) {
		MessageBox(nullptr, "CANCEL", "CANCEL", MB_OK);

	}*/

	//hal::dout << "テクスチャ\nファイル：" << FILE_NAME << "が読み込めませんでした" << std::endl;

	//return 0;
}

//-------------------------------playground-------------------------------
	//float a = -128.0f;
	//float end = 800.0f;
	//float start = -128.0f;
	//float h_start = 500.0f;
	//float d = h_start;
	//int speed = 10;
	//bool b = true;
	//int c = 0;

	//while (msg.message != WM_QUIT) {
	//	double now = SystemTimer_GetTime();
	//	double elapsed_time = now - time;
	//	time = now;
	//	SpriteAnim_Update(elapsed_time);
	//	Direct3D_Clear();
	//	Sprite_Begin();

	//	if (c == 0) {
	//		SpriteAnim_Draw(2, start, h_start, 256.0f, 256.0f);
	//	}
	//	Sprite_Draw(texid_ladder, 400.0f, 100.0f, 256.0f, 256.0f, 32.0f, 160.0f, 32.0f, 32.0f);
	//	Sprite_Draw(texid_wall, 800.0f, 100.0f, 256.0f, 256.0f, 0.0f, 160.0f, 32.0f, 32.0f);
	//	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ウィンドウメッセージが来ていたら 
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//		if (msg.message == WM_KEYDOWN) {
	//			if (msg.wParam == VK_RIGHT) {
	//				c = 1;
	//			}
	//			if (msg.wParam == VK_LEFT) {
	//				c = 2;
	//			}
	//			if (msg.wParam == VK_SPACE) {
	//				c = 3;
	//			}
	//			if (msg.wParam == VK_UP) {
	//				c = 4;
	//			}
	//			if (msg.wParam == VK_DOWN) {
	//				c = 5;
	//			}
	//		}
	//	}

	//	if (c == 1) {
	//		start = a;
	//		SpriteAnim_Draw(2, start, h_start, 256.0f, 256.0f);
	//		a += speed;
	//	}

	//	if (c == 2) {
	//		start = a;
	//		SpriteAnim_Draw(1, start, h_start, 256.0f, 256.0f);
	//		a -= speed;
	//	}
	//	
	//	if (c == 3) {
	//		SpriteAnim_Draw(3, start, h_start, 256.0f, 256.0f);
	//	}

	//	if (c == 4) {
	//		h_start = d;
	//		SpriteAnim_Draw(4, start, h_start, 256.0f, 256.0f);
	//		d -= speed;
	//	}
	//	if (c == 5) {
	//		h_start = d;
	//		SpriteAnim_Draw(4, start, h_start, 256.0f, 256.0f);
	//		d += speed;
	//	}
	//	Direct3D_Present();
	//}
	// 
	// 
	// 
	// if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ウィンドウメッセージが来ていたら 
//TranslateMessage(&msg);
//DispatchMessage(&msg);
//if (msg.message == WM_KEYDOWN) {
//	if (msg.wParam == VK_RIGHT) {
//		c = 1;
//	}
//	if (msg.wParam == VK_LEFT) {
//		c = 2;
//	}
//	if (msg.wParam == VK_SPACE) {
//		c = 3;
//	}
//	if (msg.wParam == VK_UP) {
//		c = 4;
//	}
//	if (msg.wParam == VK_DOWN) {
//		c = 5;
//	}
//}
//				}
//
//				if (c == 1) {
//					start = a;
//					SpriteAnim_Draw(pid_run_r, start, h_start, 256.0f, 256.0f);
//					a += speed;
//					current_id = pid_run_r;
//				}
//
//				if (c == 2) {
//					start = a;
//					SpriteAnim_Draw(pid_run, start, h_start, 256.0f, 256.0f);
//					a -= speed;
//					current_id = pid_run;
//				}
//
//				if (c == 3) {
//					if (current_id == pid_run_r) {
//						start = a;
//						h_start = d;
//						if (jump < 0) {
//							SpriteAnim_Draw(pid_run_r, start, h_start, 256.0f, 256.0f);
//							a += speed;
//						}
//						else if (jump > 10) {
//
//							c == current_id;
//
//						}
//						if (jump < 5) {
//
//							d -= speed;
//						}
//						else if (jump > 5 && jump < 10) {
//
//							d += speed;
//						}
//						jump++;
//						/*for (int i = 1; i < 100; i++) {
//							d -= i;
//						}*/
//						/*for (int i = 0; i < 100; i++) {
//							h_start += 1;
//						}*/
//					}
//
//					//SpriteAnim_Draw(current_id, start, h_start, 256.0f, 256.0f);
//
//					//SpriteAnim_Draw(pid_05, start, h_start, 256.0f, 256.0f);
//				}
//
//				if (c == 4) {
//					h_start = d;
//					SpriteAnim_Draw(4, start, h_start, 256.0f, 256.0f);
//					d -= speed;
//				}
//				if (c == 5) {
//					h_start = d;
//					SpriteAnim_Draw(4, start, h_start, 256.0f, 256.0f);
//					d += speed;
//				}
	//-------------------------------END of playground-------------------------------