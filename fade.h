// ========================================================================
//    Name : fade animation (fade.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/08
// ========================================================================
#ifndef FADE_H
#define FADE_H

#include <DirectXMath.h>

void Fade_Initialize();
void Fade_Finalize();
void Fade_Update(double elapsed_time);
void Fade_Draw();

void Fade_Start(double time, bool isFadeOut, DirectX::XMFLOAT3 color = { 0.0f, 0.0f, 0.0f });
void Fade_Half_Start(double time, bool isFadeOut, DirectX::XMFLOAT3 color = { 0.0f, 0.0f, 0.0f });

enum FadeState : int
{
	FADE_STATE_NONE = 0,
	FADE_STATE_FINISHED_HALF_IN,
	FADE_STATE_FINISHED_HALF_OUT,
	FADE_STATE_FINISHED_IN,
	FADE_STATE_FINISHED_OUT,
	FADE_STATE_HALF_IN,
	FADE_STATE_HALF_OUT,
	FADE_STATE_IN,
	FADE_STATE_OUT,
	FADE_STATE_MAX
};

FadeState Fade_GetState();

#endif //FADE_H