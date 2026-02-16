
// ========================================================================
//    Name : Mouse Control(mouse.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/12/16
// ========================================================================

#ifndef MOUSE_H
#define MOUSE_H

#include <windows.h>

class Mouse {
public:
	enum MOUSEMODE {
		MODE_ABSOULATE = 0,
		MODE_RELATIVE,
	};
	struct MouseState {
		int x = 0;
		int y = 0;
		int prev_x = 0;
		int prev_y = 0;
		int dx = 0;
		int dy = 0;

		bool leftDown = false;
		bool rightDown = false;
		bool middleDown = false;

		int wheelDelta = 0;

		MOUSEMODE mode = MOUSEMODE::MODE_ABSOULATE;

		bool visible = true;
	};


private:
	HWND m_hwnd = nullptr;
	MouseState state;

public:
	void Initialize(HWND hwnd);
	void ProcessMessage(UINT msg, WPARAM wp, LPARAM lp);
	void Finalize();
	void Update();

	bool IsVisible() const;
	void ClipMouseInWindow(HWND hwnd, bool enable);

	MOUSEMODE GetMode() const { return state.mode; };
	void SetMode(MOUSEMODE mode) { state.mode = mode; };

	void SetVisible(bool visible);

	int GetX() const { return state.x; };
	int GetY() const { return state.y; };
	int GetPrevX() const { return state.prev_x; };
	int GetPrevY() const { return state.prev_y; };
	int GetDX() const { return state.dx; };
	int GetDY() const { return state.dy; };
	bool GetleftDown() const { return state.leftDown; };
	bool GetRightDown() const { return state.rightDown; };
	bool GetMiddleDown() const { return state.middleDown; };
	int GetWheelDelta() const { return state.wheelDelta; };

};

void Mouse_Initialize(HWND hwnd);
void Mouse_Finalize();
void Mouse_Update();
void Mouse_ProcessMessage(UINT msg, WPARAM wp, LPARAM lp);
void Mouse_ClipMouseInWindow(HWND hwnd, bool enable);
void Mouse_SetVisible(bool visible);

Mouse& Mouse_GetMouse();
const Mouse& Mouse_GetConstMouse();

#endif // !MOUSE_H
