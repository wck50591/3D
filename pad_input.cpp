
// ========================================================================
//    Name : pad input (pad_input.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/13
// ========================================================================

#include "pad_input.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

static WORD s_prevButtons = 0;
static WORD s_nowButtons = 0;

void Pad_Reset()
{
    s_prevButtons = 0;
    s_nowButtons = 0;
}

void Pad_Update()
{
    s_prevButtons = s_nowButtons;

    XINPUT_STATE s{};
    if (XInputGetState(0, &s) != ERROR_SUCCESS) {
        s_nowButtons = 0;
        return;
    }
    s_nowButtons = s.Gamepad.wButtons;
}

bool Pad_IsPressed(std::uint16_t button)
{
    return (s_nowButtons & (WORD)button) != 0;
}

bool Pad_IsTrigger(std::uint16_t button)
{
    WORD b = (WORD)button;
    return ((s_nowButtons & b) != 0) && ((s_prevButtons & b) == 0);
}
