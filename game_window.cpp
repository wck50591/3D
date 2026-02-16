
// ========================================================================
//    Name : game window (game_window.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/06/06
// ========================================================================


#include "game_window.h"
#include <algorithm>
#include "keyboard.h"
#include "mouse.h"

#include "imgui.h"

//ウィンドウプロシージャ プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//ウィンドウ情報
static constexpr char WINDOW_CLASS[] = "GameWindow"; //メインウィンドウクラス名
static constexpr char TITLE[] = "タダノヨケルゲーム";	//タイトルバーのテキスト
static constexpr int SCREEN_WIDTH = 1600;
static constexpr int SCREEN_HEIGHT = 900;

HWND GameWindow_Create(HINSTANCE hInstance)
{
	//ウィンドウクラブの登録
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);

	//メインウィンドウの作成
	//create var for screen size
	RECT window_rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	//set window style
	DWORD style = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);

	//目的のクライアント四角形のサイズに基づいて、ウィンドウの四角形の必要なサイズを計算します。 その後、ウィンドウの四角形を CreateWindow 関数に渡して、クライアント領域が目的のサイズのウィンドウを作成できます。
	//拡張ウィンドウ スタイルを指定するには、 AdjustWindowRectEx 関数を使用します。
	AdjustWindowRect(&window_rect, style, FALSE);

	//set window size base on screen size
	const int WINDOW_WIDTH = window_rect.right - window_rect.left;
	const int WINDOW_HEIGHT = window_rect.bottom - window_rect.top;

	//デスクトップのサイズを取得
	//プライマリモニターの画面解像度取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	//ウィンドウの表示位置をデスクトップの真ん中に調整する
	const int WINDOW_X = std::max((desktop_width - WINDOW_WIDTH) / 2, 0);
	const int WINDOW_Y = std::max((desktop_height - WINDOW_HEIGHT) / 2, 0);

	HWND hWnd = CreateWindow(
		WINDOW_CLASS,
		TITLE,
		//WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		style,
		WINDOW_X,	//CW_USEDEFAULT,
		WINDOW_Y,	//0
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		nullptr, nullptr, hInstance, nullptr);


    return hWnd;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	// Pass all Win32 events to ImGui
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;   // <-- if ImGui wants to capture it, stop here

	switch (message)
	{
    case WM_ACTIVATEAPP:
		if (wParam) {
			Mouse_ClipMouseInWindow(hWnd, true);
		}
		else {
			Mouse_ClipMouseInWindow(hWnd, false);
		}
		Keyboard_ProcessMessage(message, wParam, lParam);
		Mouse_ProcessMessage(message, wParam, lParam);
		break;
	//case WM_SIZE:
	//	if (wParam == SIZE_MINIMIZED)
	//		return 0;
	//	g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
	//	g_ResizeHeight = (UINT)HIWORD(lParam);
	//	return 0;

    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    //case WM_XBUTTONDOWN:
    //case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
		Mouse_ProcessMessage(message, wParam, lParam);
        break;
    case WM_KEYDOWN:
		 if (wParam == VK_ESCAPE) {
			 SendMessage(hWnd, WM_CLOSE, 0, 0);
		 }
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
         Keyboard_ProcessMessage(message, wParam, lParam);
         break;
	case WM_CLOSE:
		if (MessageBox(hWnd, "本当に終了？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		};
		break;
	case WM_DESTROY:	//ウィンドウのメッセージ
		Mouse_ClipMouseInWindow(hWnd, false);
		PostQuitMessage(0); //WM_QUITメッセージの送信
		break;
	default:
		//通常のメッセージ処理はこの関数に任せる
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
