
// ========================================================================
//    Name : Mouse Control(mouse.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/12/16
// ========================================================================

#include "mouse.h"
#include <windowsx.h>

static Mouse g_mouse;

void Mouse::Initialize(HWND hwnd)
{

	m_hwnd = hwnd;

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hwnd, &p);

	state.x = state.prev_x = p.x;
	state.y = state.prev_y = p.y;
}

void Mouse::ProcessMessage(UINT message, WPARAM wParam, LPARAM lp)
{
    switch (message)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
		if (wParam == FALSE) {
			state.dx = state.dy = 0;
		}
		break;
    case WM_INPUT:
		break;
    case WM_MOUSEMOVE:
		state.x = GET_X_LPARAM(lp);
		state.y = GET_Y_LPARAM(lp);
        break;
	case WM_LBUTTONDOWN:
		state.leftDown = true;
		break;
	case WM_LBUTTONUP:
		state.leftDown = false;
		break;
	case WM_RBUTTONDOWN:
		state.rightDown = true;
		break;
	case WM_RBUTTONUP:
		state.rightDown = false;
		break;
	case WM_MBUTTONDOWN:
		state.middleDown = true;
		break;
	case WM_MBUTTONUP:
		state.middleDown = false;
		break;
	case WM_MOUSEWHEEL:
		state.wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		return;
	case WM_MOUSEHOVER:
		break;
	default:
		break;
    }
}

void Mouse::Finalize()
{
	
}

void Mouse::Update()
{

	state.dx = state.x - state.prev_x;
	state.dy = state.y - state.prev_y;

	state.prev_x = state.x;
	state.prev_y = state.y;
}

bool Mouse::IsVisible() const
{
	return state.visible;
}

void Mouse::ClipMouseInWindow(HWND hwnd, bool enable)
{
	if (!enable) {
		ClipCursor(nullptr);
		return;
	}

	RECT rc;
	GetClientRect(hwnd, &rc);

	POINT tl{ rc.left, rc.top };
	POINT br{ rc.right, rc.bottom };
	ClientToScreen(hwnd, &tl);
	ClientToScreen(hwnd, &br);

	RECT clip{ tl.x, tl.y, br.x, br.y };
	ClipCursor(&clip);
}

void Mouse::SetVisible(bool visible)
{
	state.visible = visible;
	ShowCursor(visible);
}

void Mouse_Initialize(HWND hwnd)
{
	g_mouse.Initialize(hwnd);
}

void Mouse_Finalize()
{
	g_mouse.Finalize();
}

void Mouse_Update()
{
	g_mouse.Update();
}

void Mouse_ProcessMessage(UINT msg, WPARAM wp, LPARAM lp)
{
	g_mouse.ProcessMessage(msg, wp, lp);
}

void Mouse_ClipMouseInWindow(HWND hwnd, bool enable)
{
	g_mouse.ClipMouseInWindow(hwnd, enable);
}

void Mouse_SetVisible(bool visible)
{
	g_mouse.SetVisible(visible);
}

Mouse& Mouse_GetMouse()
{
	return g_mouse;
}

const Mouse& Mouse_GetConstMouse()
{
	return g_mouse;
}