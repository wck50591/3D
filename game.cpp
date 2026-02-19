//ÉQÅ[ÉÄñ{ëÃ [game.cpp]
#include "game.h"
#include "scene.h"
#include "meshfield.h"
#include "grid.h"
#include "camera.h"
#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "Key_Logger.h"
#include "sampler.h"
#include "light.h"
#include "model.h"
#include "Player.h"
#include "map.h"
#include "sky.h"
//#include "bullet.h"
//#include "billboard.h"
#include "sprite.h"
#include "sprite_anim.h"
//#include "bullet_hit_effect.h"
#include "direct3d.h"
#include "cube.h"
#include "collision.h"
#include "crosshair.h"
#include "mouse.h"
#include "bridge.h"
#include "time.h"

#include "Score.h"
#include "fade.h"
#include "Audio.h"
#include "post_effect.h"

#include "pad_input.h"
#include <Xinput.h>

static float g_x = 0.0f;
static float g_angle = 0.0f;
static float g_scale = 1.0f;
static double g_AccumulatedTime = 0.0f;
static XMFLOAT3 g_CubePosition = {};
static XMFLOAT3 g_CubeVelocity = {};
static float g_car_speed = 0.1f;
static float g_car_move_z = 0.0f;
static double g_GameEndTimer = 0.0;
static float g_RuleMoveY = 0.0f;
static float g_CountDownAlpha = 1.0f;
static float g_GameTime = 0.0f;
static double g_GameTotalTime = 0.0;

static CrosshairRenderer cr;
static std::unique_ptr<Crosshair> g_crosshair;

static int g_BgmId = -1;
static int g_CarStartId = -1;
static int g_CarReverseId = -1;

static int g_TexId = -1;
static int g_AnimPatternId = -1;
static int g_AnimPlayId = -1;

static int g_RuleTexId = -1;
static int g_GameoverTexId = -1;
static int g_CountDown1TexId = -1;
static int g_CountDown2TexId = -1;
static int g_CountDown3TexId = -1;
static int g_CountDownGoTexId = -1;

static int g_ResultTexId = -1;
static int g_ResultSTexId = -1;
static int g_ResultATexId = -1;
static int g_ResultBTexId = -1;
static int g_ResultCTexId = -1;
static int g_ResultOKTexId = -1;

static bool g_GameStart = false;
static bool g_GameStartFadeIn = false;
static bool g_WaitInput = false;
static bool g_ShowRule = false;
static bool g_RuleMove = false;
static bool g_ShowCountDown = false;
static bool g_Show1 = false;
static bool g_Show2 = false;
static bool g_Show3 = false;
static bool g_ShowGo = false;

static bool g_ShowResult = false;
static bool g_ShowS = false;
static bool g_ShowA = false;
static bool g_ShowB = false;
static bool g_ShowC = false;

static bool g_IsDebug = false;

void Game_Initialize()
{
	Pad_Reset();
	Camera_Initialize();

	g_BgmId = LoadAudio("resource/sound/close danger.wav");
	g_CarStartId = LoadAudio("resource/sound/car_start.wav");
	g_CarReverseId = LoadAudio("resource/sound/car_reverse.wav");

	g_RuleTexId = Texture_Load(L"resource/texture/rule.png", true);
	g_GameoverTexId = Texture_Load(L"resource/texture/gameover.png", true);
	g_CountDown1TexId = Texture_Load(L"resource/texture/1.png", true);
	g_CountDown2TexId = Texture_Load(L"resource/texture/2.png", true);
	g_CountDown3TexId = Texture_Load(L"resource/texture/3.png", true);
	g_CountDownGoTexId = Texture_Load(L"resource/texture/go.png", true);

	g_ResultTexId =  Texture_Load(L"resource/texture/result.png", true);
	g_ResultSTexId = Texture_Load(L"resource/texture/s.png", true);
	g_ResultATexId = Texture_Load(L"resource/texture/a.png", true);
	g_ResultBTexId = Texture_Load(L"resource/texture/b.png", true);
	g_ResultCTexId = Texture_Load(L"resource/texture/c.png", true);
	g_ResultOKTexId = Texture_Load(L"resource/texture/end_button.png", true);

	g_IsDebug = false;
	g_GameStart = false;
	g_GameStartFadeIn = false;
	g_WaitInput = false;
	g_ShowRule = false;
	g_RuleMove = false;
	g_ShowCountDown = false;
	g_Show1 = false;
	g_Show2 = false;
	g_Show3 = false;
	g_ShowGo = false;

	g_ShowResult = false;
	g_ShowS = false;
	g_ShowA = false;
	g_ShowB = false;
	g_ShowC = false;

	/*Camera_Initialize(
		{ 1.1f,1.0f,-0.7f },
		{ 0.0f,0.0f,1.0f, },
		{ 1.0f,0.0f,0.0f });*/

	Player_Initialize({0.0f,0.0f,-15.0f}, {0.0f,0.0f,1.0f});
	PlayerCamera_Initialize();
	
	Map_Initialize();
	Sky_Initialize();
	Score_Initialize();

	g_AnimPatternId = SpriteAnim_RegisterPattern(
		g_TexId, 10, 5, 0.1, { 140, 200}, {0, 0,}
	);

	g_AnimPlayId = SpriteAnim_CreatePlayer(g_AnimPatternId);

	g_car_speed = 5.0f;
	g_RuleMoveY = 0.0f;
	g_CountDownAlpha = 1.0f;
	g_GameTime = 0.0f;
	g_GameEndTimer = 0.0f;

	g_GameTotalTime = 0.0;
	Score_Reset();
	Score_ShowScore(0);

	PostEffect_SetGamma(1.0f);
	PostEffect_SetInverseColor(0.0f);
}

void Game_Finalize()
{
	Score_Finalize();
	Sky_Finalize();
	Map_Finalize();
	PlayerCamera_Finalize();
	Player_Finalize();
	if (g_crosshair) {
		g_crosshair->Finalize();
		g_crosshair.reset();
	}
	UnloadAudio(g_CarReverseId);
	UnloadAudio(g_CarStartId);
	UnloadAudio(g_BgmId);
	Camera_Finalize();
}

void Game_Update(double elapsed_time)
{
	Time_Update(elapsed_time);
	Pad_Update();
	
	//if (!g_GameStart && Fade_GetState() == FADE_STATE_FINISHED_OUT) {
	if (!g_GameStart && !g_GameStartFadeIn) {
		PlayAudio(g_BgmId, true);
		Fade_Start(2.0, false); // fade IN
		g_ShowRule = true;
		g_GameStartFadeIn = true;
		//Fade_Half_Start(2.0, false);
	}

	if (Fade_GetState() == FADE_STATE_FINISHED_IN) {
		// show rule, wait for enter
		g_WaitInput = true;
	}
	if (g_WaitInput) {
		if (KeyLogger_IsTrigger(KK_ENTER) || Pad_IsTrigger((uint16_t)XINPUT_GAMEPAD_A)) {
			g_WaitInput = false;
			g_RuleMove = true;
		}
	}

	if (g_RuleMove) {
		g_RuleMoveY += (float)(elapsed_time * 1000.0f);
		// show rule end
		if (g_RuleMoveY >= 1200.0f) {
			g_RuleMove = false;
			g_ShowRule = false;
			g_ShowCountDown = true;
		}
	}
	
	if (g_ShowCountDown) {
		g_Show3 = true;
		g_ShowCountDown = false;
		PlayAudio(g_CarStartId, false);
	}

	float countdown_speed = 0.8f;

	if (g_Show3) {
		g_CountDownAlpha -= (float)(elapsed_time * countdown_speed);
		if (g_CountDownAlpha <= 0.0f) {
			g_Show3 = false;
			g_Show2 = true;
			g_CountDownAlpha = 1.0f;
		}
	}

	if (g_Show2) {
		g_CountDownAlpha -= (float)(elapsed_time * countdown_speed);
		if (g_CountDownAlpha <= 0.0f) {
			g_Show2 = false;
			g_Show1 = true;
			g_CountDownAlpha = 1.0f;
		}
	}

	if (g_Show1) {
		g_CountDownAlpha -= (float)(elapsed_time * countdown_speed);
		if (g_CountDownAlpha <= 0.0f) {
			g_Show1 = false;
			g_ShowGo = true;
			g_CountDownAlpha = 1.0f;
		}
	}

	if (g_ShowGo) {
		g_CountDownAlpha -= (float)(elapsed_time * countdown_speed);
		// game start
	
		Player_SetStart(true);
		g_GameStart = true;
		
		if (g_CountDownAlpha <= 0.0f) {
			g_ShowGo = false;
			g_ShowCountDown = false;
		}
	}

	static double g_ScoreAcc = 0.0;

	if (g_GameStart) {
		
		g_GameTotalTime += elapsed_time;

		const double scorePerSecond = 100.0;
		g_ScoreAcc += elapsed_time * scorePerSecond;

		int add = (int)g_ScoreAcc;
		if (add > 0) {
			Score_AddScore(0, add);
			g_ScoreAcc -= add;
		}

		g_GameTime += static_cast<float>(elapsed_time);
		if (g_GameTime >= 10.0f) {
			float speed = Player_GetCarSpeed();
			speed -= 0.1f;
			Player_SetCarSpeed(speed);
			g_GameTime = 0.0f;
		}
	}
	g_AccumulatedTime += elapsed_time;

	g_x = (float)(sin(g_AccumulatedTime)) * 4.5f;
	g_angle = (float)(g_AccumulatedTime * 3.0f);
	g_scale = (float)(sin(g_AccumulatedTime) * 5.0f);
	g_scale = (float)((sin(g_AccumulatedTime) + 1.0f) * 0.5f * 5.0f);

	Player_Update(elapsed_time);
	Sky_SetPosition(Player_GetPosition());
	if (g_IsDebug) {
		Camera_Update(elapsed_time);
	}
	else {
		XMFLOAT3 p = Player_GetPosition();
		XMFLOAT3 position = { p.x,p.y + 2.0f ,p.z + (-3.0f) };
		PlayerCamera_SetPosition(position);
		XMFLOAT3 target = { p.x,p.y + 1.0f ,p.z };
		PlayerCamera_SetTargetPosition(target);
		PlayerCamera_Update(elapsed_time);
	}

	if (KeyLogger_IsTrigger(KK_F1)) {
		g_IsDebug = !g_IsDebug;
	}

	if (Player_IsGameover()){
		g_GameStart = false;
		g_GameEndTimer += elapsed_time;
		if (g_GameEndTimer >= 5.0) {
			g_ShowResult = true;
		}
	}

	if (g_ShowResult) {

		if (g_GameTotalTime > 90.0) {
			g_ShowS = true;
		}
		else if (g_GameTotalTime > 60.0) {
			g_ShowA = true;
		}
		else if (g_GameTotalTime > 30.0) {
			g_ShowB = true;
		}
		else {
			g_ShowC = true;
		}

		if (KeyLogger_IsTrigger(KK_ENTER) || Pad_IsTrigger((uint16_t)XINPUT_GAMEPAD_A)) {
			Scene_Change(SCENE_TITLE);
			return;
		}
	}

	XMFLOAT3 p = Player_GetPosition();

	//const float HALF = 25.0f;
	//const float SIZE = HALF * 2.0f;

	const float HALFa = 25.0f;
	const float SIZEa = 50.0f;

	float fx = floorf((p.x + HALFa) / SIZEa) * SIZEa;
	float fz = floorf((p.z + HALFa) / SIZEa) * SIZEa;

	Meshfield_SetPositionX(fx);
	Meshfield_SetPositionZ(fz);

	Score_Update();
}

void Game_Draw()
{

	XMFLOAT4X4 mtxView = g_IsDebug ? Camera_GetMatrix() : PlayerCamera_GetViewMatrix();

	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = g_IsDebug ? XMLoadFloat4x4(&Camera_GetPerspectiveMatrix()) : XMLoadFloat4x4(&PlayerCamera_GetPerspectiveMatrix());

	XMFLOAT3 camera_position = g_IsDebug ? Camera_GetPosition() : PlayerCamera_GetPosition();

	Camera_SetMatrix(view, proj);

	light_SetAmbient({0.2f, 0.2f, 0.2f});
	light_SetDirectionalWorld({ 0.0f, -1.0f, 0.0f, 0.0f }, { 0.2f,0.2f,0.2f, 1.0f });

	Direct3D_SetDepthEnable(false);
	Sky_Draw();
	Direct3D_SetDepthEnable(true);

	
#if defined(DEBUG) || defined(_DEBUG)
	Collision_DebugDraw(Player_GetAABB(), { 1.0f,0.0f,0.0f,1.0f });
#endif
	Sampler_SetFilterAnisotropic();

	Map_Draw();

	Light_SetSpecularWorld(camera_position, 50.0f, { 1.0f,1.0f,1.0f,1.0f });

}

void Game_DrawNormal()
{
	// draw after post effect (no effect)

	Player_Draw();

	Map_DrawNormal();

	Direct3D_SetDepthDepthWriteDisable();

	Direct3D_SetDepthEnable(false);

	if (Player_IsGameover() && !g_ShowResult) {
		Sprite_Draw(g_GameoverTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_GameoverTexId) * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_GameoverTexId) * 0.5f,
			static_cast<float>(Texture_Width(g_GameoverTexId)),
			static_cast<float>(Texture_Height(g_GameoverTexId))
		);
	}

	Score_Draw();

	float scale = 0.8f;

	if (g_ShowRule) {
		Sprite_Draw(g_RuleTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_RuleTexId) * scale * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_RuleTexId) * scale * 0.5f - g_RuleMoveY,
			Texture_Width(g_RuleTexId) * scale,
			Texture_Height(g_RuleTexId) * scale
		);
	}

	if (g_Show3) {
		Sprite_Draw(g_CountDown3TexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_CountDown3TexId) * scale * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_CountDown3TexId) * scale * 0.5f,
			Texture_Width(g_CountDown3TexId) * scale,
			Texture_Height(g_CountDown3TexId) * scale,
			{ 1.0f, 1.0f, 1.0f, g_CountDownAlpha }
		);
	}
	if (g_Show2) {
		Sprite_Draw(g_CountDown2TexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_CountDown2TexId) * scale * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_CountDown2TexId) * scale * 0.5f,
			Texture_Width(g_CountDown2TexId) * scale,
			Texture_Height(g_CountDown2TexId) * scale,
			{ 1.0f, 1.0f, 1.0f, g_CountDownAlpha }
		);
	}
	if (g_Show1) {
		Sprite_Draw(g_CountDown1TexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_CountDown1TexId) * scale * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_CountDown1TexId) * scale * 0.5f,
			Texture_Width(g_CountDown1TexId) * scale,
			Texture_Height(g_CountDown1TexId) * scale,
			{ 1.0f, 1.0f, 1.0f, g_CountDownAlpha }
		);
	}
	if (g_ShowGo) {
		Sprite_Draw(g_CountDownGoTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_CountDownGoTexId) * scale * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_CountDownGoTexId) * scale * 0.5f,
			Texture_Width(g_CountDownGoTexId) * scale,
			Texture_Height(g_CountDownGoTexId) * scale
		);
	}

	if (g_ShowResult) {
		Sprite_Draw(g_ResultTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_ResultTexId) * scale * 0.5f,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_ResultTexId) * scale * 0.5f,
			Texture_Width(g_ResultTexId) * scale,
			Texture_Height(g_ResultTexId) * scale
		);
	}

	float result_offset_x = 200.0f;
	float result_offset_y = 30.0f;

	if (g_ShowS) {
		Sprite_Draw(g_ResultSTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_ResultSTexId) * scale * 0.5f + result_offset_x,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_ResultSTexId) * scale * 0.5f + result_offset_y,
			Texture_Width(g_ResultSTexId) * scale,
			Texture_Height(g_ResultSTexId) * scale
		);
	}
	if (g_ShowA) {
		Sprite_Draw(g_ResultATexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_ResultATexId) * scale * 0.5f + result_offset_x,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_ResultATexId) * scale * 0.5f + result_offset_y,
			Texture_Width(g_ResultATexId) * scale,
			Texture_Height(g_ResultATexId) * scale
		);
	}
	if (g_ShowB) {
		Sprite_Draw(g_ResultBTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_ResultBTexId) * scale * 0.5f + result_offset_x,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_ResultBTexId) * scale * 0.5f + result_offset_y,
			Texture_Width(g_ResultBTexId) * scale,
			Texture_Height(g_ResultBTexId) * scale
		);
	}
	if (g_ShowC) {
		Sprite_Draw(g_ResultCTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_ResultCTexId) * scale * 0.5f + result_offset_x,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_ResultCTexId) * scale * 0.5f + result_offset_y,
			Texture_Width(g_ResultCTexId) * scale,
			Texture_Height(g_ResultCTexId) * scale
		);
	}

	result_offset_x = 0.0f;
	result_offset_y = 300.0f;

	if (g_ShowResult) {
		Sprite_Draw(g_ResultOKTexId,
			Direct3D_GetBackBufferWidth() * 0.5f - Texture_Width(g_ResultOKTexId) * scale * 0.5f + result_offset_x,
			Direct3D_GetBackBufferHeight() * 0.5f - Texture_Height(g_ResultOKTexId) * scale * 0.5f + result_offset_y,
			Texture_Width(g_ResultOKTexId) * scale,
			Texture_Height(g_ResultOKTexId) * scale
		);
	}

	Direct3D_SetDepthEnable(true);

	if (g_IsDebug) {
		Camera_DebugDraw();
	}
}
