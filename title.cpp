// title.cpp
#include "title.h"

#include "my_math.h"
#include "camera.h"
#include "player_camera.h"
#include "Audio.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "scene.h"
#include "direct3d.h"
#include "map.h"
#include "fade.h"
#include "light.h"
#include "sky.h"
#include "player.h"
#include "catmull_rom_spline.h"

#include "pad_input.h"
#include <Xinput.h>
#include "post_effect.h"
#include "sampler.h"

static int g_TitleBgTexId = -1;
static int g_TitleStartTexId = -1;
static int g_BgmId = -1;
static int g_EnterSeId = -1;

static bool g_IsDebug = false;
static bool g_TitleFadeStart = false;
static bool g_WaitPadRelease = false;

CatmullRomSpline crp;
// time in each part
static float g_CrpProgress = 0.0f;
// time in one cycle
static float g_CrpDuration = 0.0f;

static void CameraRowMovementSetup();


void Title_Initialize()
{
	Pad_Reset();
	g_WaitPadRelease = true;

	Camera_Initialize();

	PostEffect_SetGamma(1.0f);
	PostEffect_SetInverseColor(0.0f);

	Player_Initialize({ 0.0f,0.0f,-15.0f }, { 0.0f,0.0f,1.0f });
	PlayerCamera_Initialize();
	PlayerCamera_SetPosition(Player_GetPosition());
	
	g_CrpProgress = 0.0f;
	g_CrpDuration = 10.0f;
	CameraRowMovementSetup();
	
	Map_Initialize();
	Sky_Initialize();
	g_BgmId = LoadAudio("resource/sound/waking beat.wav");
	g_EnterSeId = LoadAudio("resource/sound/start_engine.wav");
	PlayAudio(g_BgmId, true);

	g_IsDebug = false;
	g_TitleFadeStart = false;
	g_TitleBgTexId = Texture_Load(L"resource/texture/game_title.png",TRUE);
	g_TitleStartTexId = Texture_Load(L"resource/texture/start.png",TRUE);

}

void Title_Finalize()
{
	//Texture_AllRelease();
	UnloadAudio(g_EnterSeId);
	UnloadAudio(g_BgmId);
	Sky_Finalize();
	Map_Finalize();
	Player_Finalize();
	PlayerCamera_Finalize();
	Camera_Finalize();
	
}

void Title_Update(double elapsed_time)
{

	g_CrpProgress += (float)elapsed_time / g_CrpDuration;
	g_CrpProgress -= floorf(g_CrpProgress); // keep 0..1 (loop)
	//g_CrpProgress = clamp(g_CrpProgress, 0.0f, 1.0f);  // only 0..1 once (not loop)

	Pad_Update();
	//Player_Update(elapsed_time);
	if (g_WaitPadRelease)
	{
		if (!Pad_IsPressed((uint16_t)XINPUT_GAMEPAD_A)) {
			g_WaitPadRelease = false;
		}
	}
	if (!g_WaitPadRelease && !g_TitleFadeStart &&
		(KeyLogger_IsTrigger(KK_ENTER) || Pad_IsTrigger((uint16_t)XINPUT_GAMEPAD_A)))
	{
		Fade_Start(2.0, true); // fade OUT
		PlayAudio(g_EnterSeId, false);
		
		g_TitleFadeStart = true;
	}

	if (g_TitleFadeStart) {
		AudioFade(g_EnterSeId, static_cast<unsigned short>(elapsed_time / 3.0f));
		AudioFade(g_BgmId, static_cast<unsigned short>(elapsed_time / 3.0f));
	}

    if (Fade_GetState() == FADE_STATE_FINISHED_OUT)
    {
		g_TitleFadeStart = false;
        Scene_Change(SCENE_GAME);
    }

	static float z = 0.0f;
	z += 5.0f * (float)elapsed_time;
	
	if (g_IsDebug) {
		Camera_Update(elapsed_time);
	}
	else {
		// move the camera around the player position
		// using catmull-rom spline

		XMFLOAT3 p = Player_GetPosition();
		XMFLOAT3 off = crp.GetPosition(g_CrpProgress);
		float offY = 0.5f;

		// move the camera a bit higher
		XMFLOAT3 camPos = { p.x + off.x, p.y + offY, p.z + off.z };

		PlayerCamera_SetPosition(camPos);
		PlayerCamera_SetTargetPosition(p);
		PlayerCamera_Update(elapsed_time);
	}

	Sky_SetPosition(Player_GetPosition());
}

void Title_Draw()
{

	XMFLOAT4X4 mtxView = g_IsDebug ? Camera_GetMatrix() : PlayerCamera_GetViewMatrix();

	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = g_IsDebug ? XMLoadFloat4x4(&Camera_GetPerspectiveMatrix()) : XMLoadFloat4x4(&PlayerCamera_GetPerspectiveMatrix());

	XMFLOAT3 camera_position = g_IsDebug ? Camera_GetPosition() : PlayerCamera_GetPosition();

	Camera_SetMatrix(view, proj);

	Sampler_SetFilterAnisotropic();

	Direct3D_SetDepthEnable(false);
	Sky_Draw();
	Direct3D_SetDepthEnable(true);


	Player_Draw();

	Map_Draw();
	Map_DrawNormal();

	light_SetAmbient({ 0.2f, 0.2f, 0.2f });
	light_SetDirectionalWorld({ 0.0f, -1.0f, 0.0f, 0.0f }, { 0.2f,0.2f,0.2f, 1.0f });

	float width_center = (float)Direct3D_GetBackBufferWidth()   * 0.5f;
	float height_center = (float)Direct3D_GetBackBufferHeight() * 0.5f;

	float scale = 0.7f;

	float tex_w = Texture_Width(g_TitleBgTexId) * scale;
	float tex_h = Texture_Height(g_TitleBgTexId) * scale;

	float offset_h = 150.0f;

	// draw 2d
	Direct3D_SetDepthEnable(false);

	Sprite_Draw(g_TitleBgTexId,
		width_center  - tex_w * 0.5f,
		height_center - tex_h * 0.5f - offset_h,
		tex_w,
		tex_h
	);

	scale = 0.4f;

	tex_w = Texture_Width(g_TitleStartTexId) * scale;
	tex_h = Texture_Height(g_TitleStartTexId) * scale;

	offset_h = 300.0f;

	Sprite_Draw(g_TitleStartTexId,
		width_center - tex_w * 0.5f,
		height_center - tex_h * 0.5f + offset_h,
		tex_w,
		tex_h
	);

}

static void CameraRowMovementSetup() {

	crp.Clear();

	const int N = 16;      // must be >= 8 and 4's multiply, higher is smoother
	const float radius = 1.0f; // how far away from player

	// N points with 3 points in the end for last segment for looping
	for (int i = 0; i < N + 3; ++i) 
	{
		int point = i % N;						// fix into 0..N-1
		float percent = point / (float)N;		// get the points percent
		float angle = XM_2PI * percent + XM_PIDIV2;			// get the angle in circle

		// set a point's position
		// 
		// e.g : N = 16
		// i = 0,  off =  1*r, 0.0f,  0*r  (+X -> right) 
		// i = 4,  off =  0*r, 0.0f,  1*r  (+Z -> front)
		// i = 8,  off = -1*r, 0.0f,  0*r  (-X -> left)
		// i = 12, off =  0*r, 0.0f, -1*r  (-Z -> back)

		XMFLOAT3 p = { cosf(angle) * radius, 0.0f, sinf(angle) * radius };

		crp.AddPoint(p);
	}
}