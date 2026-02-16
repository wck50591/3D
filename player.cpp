
// ========================================================================
//    Name : Player(player.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/10/31
// ========================================================================
#include "player.h"
#include <directXMath.h>
using namespace DirectX;
#include "Model.h"
#include "key_logger.h"
#include "light.h"
#include "player_camera.h"
#include "map.h"
#include "cube.h"
#include "bullet.h"
#include "sprite.h"
#include "texture.h"
#include "Audio.h"
#include "crosshair.h"
#include "my_math.h"
#include "post_effect.h"

#include "Xinput.h"
#pragma comment(lib, "xinput.lib")


static constexpr float TILE = 50.0f;
static constexpr float HALF_X = 0.2f;
static constexpr float HALF_Z = 0.5f;
static constexpr float HEIGHT = 0.5f;
static constexpr int SLOW_NUMBER = 3;
static constexpr double SLOW_INVERSE_TIME = 3.0;

static int g_CarTurnId = -1;
static int g_CarMovingId = -1;
static int g_CarStopId = -1;
static int g_CarCrashId = -1;

static XMFLOAT3 g_PlayerPosition = {};
static XMFLOAT3 g_PlayerFront = { 0.0f, 0.0f, 1.0f };
static XMFLOAT3 g_PlayerVelocity = {};
static MODEL* g_pPlayerModel = { nullptr };

static bool g_IsJump = false;
static bool g_IsStart = false;
static bool g_IsCarCrash = false;
static bool g_IsCarFront = true;
static bool g_IsGameover = false;
static bool g_LeftStickMoving = false;

static float g_CarSpeed = 1.5f;

static float g_TimeSlowScale = 1.0f;
static bool g_IsSlowMo = false;
static double g_SlowTime = 0.0f;
static int g_SlowTimeNumber = SLOW_NUMBER;
static float g_SlowInverseStrength = 0.0f;

static float GetLeftStickX();

void Player_Initialize(const XMFLOAT3& position, const XMFLOAT3& front)
{


	g_CarTurnId = LoadAudio("resource/sound/car_turn.wav");
	g_CarMovingId = LoadAudio("resource/sound/car_moving.wav");
	g_CarStopId = LoadAudio("resource/sound/car_stop.wav");
	g_CarCrashId = LoadAudio("resource/sound/car_crash.wav");

	g_PlayerPosition = position;
	g_PlayerVelocity = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&g_PlayerFront, XMVector3Normalize(XMLoadFloat3(&front)));
	g_IsJump = false;

	g_IsGameover = false;
	g_IsStart = false;
	g_IsCarCrash = false;
	g_IsCarFront = true;

	g_CarSpeed = 1.5f;

	g_TimeSlowScale = 1.0f;
	g_IsSlowMo = false;
	g_SlowTime = 0.0f;
	g_SlowTimeNumber = SLOW_NUMBER;
	g_SlowInverseStrength = 0.0f;

	g_pPlayerModel = ModelLoad("resource/model/car_body.fbx", 0.1f, false);
}

void Player_Finalize()
{
	UnloadAudio(g_CarCrashId);
	UnloadAudio(g_CarStopId);
	UnloadAudio(g_CarMovingId);
	UnloadAudio(g_CarTurnId);
	ModelRelease(g_pPlayerModel);
}

void Player_Update(double elapsed_time)
{
	double o_elapsed = elapsed_time;
	double s_elapsed = elapsed_time;

	XMVECTOR position = XMLoadFloat3(&g_PlayerPosition);
	XMVECTOR velocity = XMLoadFloat3(&g_PlayerVelocity);
	XMVECTOR gvelocity{};

	

	//ï˚å¸

	XMVECTOR direction = {};
	XMVECTOR front = XMLoadFloat3(&PlayerCamera_GetFront()) * XMVECTOR { 1.0f, 0.0f, 1.0f };
	if (g_IsStart) {
		PlayAudio(g_CarMovingId, false);
		direction += front;

		//------------------------
		// SLOW MOTION
		//------------------------

		if (KeyLogger_IsTrigger(KK_SPACE)) {
			if (!g_IsSlowMo && g_SlowTimeNumber != 0) {
				g_SlowTimeNumber--;
				g_IsSlowMo = !g_IsSlowMo;
			}
		}

		if (g_IsSlowMo) {

			g_SlowTime += o_elapsed;

			float fade = 0.5f;
			float fadeOutTime = SLOW_INVERSE_TIME - fade;

			if (g_SlowTime < fade) { // < 1

				// fade in
				float t = static_cast<float>(g_SlowTime) / fade;
				t = clamp(t, 0.0f, 1.0f);
				g_SlowInverseStrength = EaseOutQuint(t);

			}
			else if (g_SlowTime < fadeOutTime) { // < 2

				g_SlowInverseStrength = 1.0f;
			}
			
			else if (g_SlowTime < SLOW_INVERSE_TIME) { // < 3

				// fade out
				float t = static_cast<float>(g_SlowTime) - fadeOutTime; 
				t /= fade;
				t = clamp(t, 0.0f, 1.0f);
				g_SlowInverseStrength = 1.0f - EaseOutQuint(t);
			}
			else {
				g_IsSlowMo = !g_IsSlowMo;
				g_SlowTime = 0.0;
				g_SlowInverseStrength = 0.0f;
			}

			PostEffect_InverseColor(g_SlowInverseStrength);
		}

		float target = g_IsSlowMo ? 0.25f : 1.0f;
		g_TimeSlowScale += (target - g_TimeSlowScale) * 6.0f * (float)o_elapsed;

		s_elapsed = o_elapsed * g_TimeSlowScale;

		//------------------------
		// END SLOW MOTION
		//------------------------


		//------------------------
		// MOVE CONTROLS 
		//------------------------

		float axisX = 0.0f;
		static bool s_MovingSound = false;

		// keyboard
		if (KeyLogger_IsPressed(KK_A)) axisX -= 1.0f;
		if (KeyLogger_IsPressed(KK_D)) axisX += 1.0f;

		// stick
		float lx = GetLeftStickX();
		axisX += lx;

		// clamp
		axisX = std::max(-1.0f, std::min(1.0f, axisX));

		static bool s_Turning = false;

		bool turningNow = (fabs(axisX) > 0.05f); // small threshold

	
		XMVECTOR right = XMVector3Normalize(
			XMVector3Cross({ 0.0f, 1.0f, 0.0f }, front)
		);

		direction += right * axisX;

		if (turningNow && !s_Turning)
		{
			g_IsCarFront = false;
			PlayAudio(g_CarTurnId, false);
		}

		XMVECTOR moveXZ = direction * XMVECTOR{ 1.0f, 0.0f, 1.0f };
		float moveLenSq = XMVectorGetX(XMVector3LengthSq(moveXZ));

		bool movingForward = false;

		if (moveLenSq > 0.0001f)
		{
			float dot = XMVectorGetX(
				XMVector3Dot(XMVector3Normalize(moveXZ), front)
			);

			movingForward = (dot > 0.7f); // threshold
		}

		if (movingForward && !s_MovingSound)
		{
			g_IsCarFront = true;
			PlayAudio(g_CarMovingId, true);
		}

		if (!movingForward && s_MovingSound)
		{
			g_IsCarFront = false;
			StopAudio(g_CarMovingId);
		}

		s_MovingSound = movingForward;
		s_Turning = turningNow;
		
	}
		//------------------------
		// END of MOVE CONTROLS 
		//------------------------

	//èdóÕ
	XMVECTOR gdir = { 0.0f, 1.0f, 0.0f };
	velocity += gdir * -9.8f * 5.0f * (float)s_elapsed;
	//gvelocity = velocity * (float)elapsed_time;
	position += velocity * (float)s_elapsed;

	if (XMVectorGetY(position) < 0.0f) {
		position = XMVectorSetY(position, 0.0f);
		velocity *= XMVECTOR{ 1,0,1,0 };
		g_IsJump = false;
	}

	if (XMVectorGetX(XMVector3LengthSq(direction)) > 0.0f) {
		direction = XMVector3Normalize(direction);
		//XMStoreFloat3(&g_PlayerFront, direction);
		
		//ÇQÇ¬ÇÃÉxÉNÉgÉãÇÃÇ»Ç∑äpÇÕ
		float dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&g_PlayerFront), direction));
		float angle = acosf(dot);

		//âÒì]ë¨ìx
		const float ROTATION_SPEED = XM_2PI * 2.0f * (float)s_elapsed;

		if (angle < ROTATION_SPEED) {
			front = direction;
		}
		else {
			//å¸Ç´ÇΩÇ¢ï˚å¸Ç™âEâÒÇËÇ©ç∂âÒÇËÇ©
			XMMATRIX rotation = XMMatrixIdentity();

			if (XMVectorGetY(XMVector3Cross(XMLoadFloat3(&g_PlayerFront), direction)) < 0.0f) {
				rotation = XMMatrixRotationY(-ROTATION_SPEED);
			}
			else {
				rotation = XMMatrixRotationY(ROTATION_SPEED);
			}

			front = XMVector3TransformNormal(XMLoadFloat3(&g_PlayerFront), rotation);
			
		}
		velocity += front * (float)(300.0f / 50.0f * s_elapsed);
		XMStoreFloat3(&g_PlayerFront, front);
	}

	velocity += -velocity * (float)(g_CarSpeed * s_elapsed);
	position += velocity * (float)s_elapsed;

	// player AABB for current predicted position
	AABB player = Player_ConvertPositionToAABB(position);

	XMFLOAT3 posF;
	XMStoreFloat3(&posF, position);
	float baseZ = floorf(posF.z / TILE) * TILE;

	// check three tiles: behind/current/ahead (same as your rendering)
	for (int t = -1; t <= 1; ++t)
	{
		float cz = baseZ + t * TILE;

		for (int i = 0; i < Map_GetObjectsCount(); ++i)
		{
			const MapObject* mo = Map_GetObject(i);

			// collide only with solid stuff
			if (mo->TypeId == TREE || mo->TypeId == ROCK) continue;

			//AABB object = Cube_GetAABB(mo->Position);
			AABB object = mo->Aabb;
			object.min.z += cz;
			object.max.z += cz;

			Hit hit = Collision_IsHitAABB(object, player);
			if (hit.isHit)
			{
				if (hit.normal.x > 0.0f) {
					position = XMVectorSetX(position, object.max.x + HALF_X);
					velocity *= {0.0f, 1.0f, 1.0f}; // Ç¢ÇÎÇÒÇ»ïœâª
				}
				else if (hit.normal.x < 0.0f) {
					position = XMVectorSetX(position, object.min.x - HALF_X);
					velocity *= {0.0f, 1.0f, 1.0f};
				}
				else if (hit.normal.y < 0.0f) {
					position = XMVectorSetY(position, object.min.y - HEIGHT);
					velocity *= {1.0f, 0.0f, 1.0f};
				}
				else if (hit.normal.z > 0.0f) {
					position = XMVectorSetZ(position, object.max.z + HALF_Z);
					velocity *= {1.0f, 1.0f, 0.0f};
				}
				else if (hit.normal.z < 0.0f) {
					position = XMVectorSetZ(position, object.min.z - HALF_Z);
					velocity *= {1.0f, 1.0f, -0.1f};
					if (g_IsCarFront) {
						StopAudio(g_CarMovingId);
					}
					g_IsCarCrash = true;
					g_IsStart = false;
					g_IsGameover = true;
				}
			}
		}
	}

	if (g_IsCarCrash) {
		PlayAudio(g_CarCrashId, false);
		PlayAudio(g_CarStopId, false);
		PlayerCamera_Shake(1.0f);
		g_IsCarCrash = false;
	}

	XMStoreFloat3(&g_PlayerPosition, position);
	XMStoreFloat3(&g_PlayerVelocity, velocity);


	/*if (KeyLogger_IsTrigger(KK_SPACE)) {
		XMFLOAT3 velocity;
		XMFLOAT3 shot_position = g_PlayerPosition;
		shot_position.y += 1.0f;
		shot_position.x += 1.0f;

		XMStoreFloat3(&velocity, XMLoadFloat3(&g_PlayerFront) * 10.0f);
		Bullet_Create(shot_position, velocity);
	}*/

}

void Player_Draw()
{
	XMFLOAT3 pos = PlayerCamera_GetPosition();
	pos.z += 5.0f;

	Light_SetSpecularWorld(pos, 10.0f, {1.0f,1.0f,1.0f,1.0f});

	/*float dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&g_PlayerFront), XMVECTOR{ 1.0f,0.0f,0.0f }));
	float angle = acosf(dot) * XMConvertToDegrees(270);*/

	float angle = -atan2f(g_PlayerFront.z, g_PlayerFront.x) + XMConvertToRadians(270);

	XMMATRIX rot = XMMatrixRotationY(angle);
	XMMATRIX trans = XMMatrixTranslation(g_PlayerPosition.x, g_PlayerPosition.y, g_PlayerPosition.z);
	XMMATRIX world = XMMatrixRotationY(XMConvertToRadians(180)) * rot * trans;

	//XMMATRIX trans = XMMatrixTranslation(g_PlayerPosition.x, g_PlayerPosition.y + 1.0f, g_PlayerPosition.z);
	//XMMATRIX world = trans;

	ModelDraw(g_pPlayerModel, world);

}

const DirectX::XMFLOAT3& Player_GetPosition()
{
	return g_PlayerPosition;
}

const DirectX::XMFLOAT3& Player_GetFront()
{
	return g_PlayerFront;

}

AABB Player_GetAABB()
{
	return {
		{
			g_PlayerPosition.x - HALF_X,
			g_PlayerPosition.y,
			g_PlayerPosition.z - HALF_Z
		},
		{
			g_PlayerPosition.x + HALF_X,
			g_PlayerPosition.y + HEIGHT,
			g_PlayerPosition.z + HALF_Z
		}
	};
	/*return {
		{
			g_PlayerPosition.x - 1.0f,
			g_PlayerPosition.y,
			g_PlayerPosition.z - 1.0f
		},
		{
			g_PlayerPosition.x + 1.0f,
			g_PlayerPosition.y + 1.0f,
			g_PlayerPosition.z + 1.0f
		}
	};*/
}

AABB Player_ConvertPositionToAABB(const DirectX::XMVECTOR& position)
{
	AABB aabb;

	XMStoreFloat3(&aabb.min, position - XMVECTOR{ HALF_X, 0.0f,   HALF_Z });
	XMStoreFloat3(&aabb.max, position + XMVECTOR{ HALF_X, HEIGHT, HALF_Z });

	//XMStoreFloat3(&aabb.min, position - XMVECTOR{ 1.0f, 0.0f, 1.0f});
	//XMStoreFloat3(&aabb.max, position + XMVECTOR{ 1.0f, 2.0f, 1.0f});

	return aabb;
}

bool Player_IsGameover()
{
	return g_IsGameover;
}

bool Player_IsStart() {
	return g_IsStart;
}

void Player_SetStart(bool b) {
	g_IsStart = b;
}

float Player_GetCarSpeed()
{
	return g_CarSpeed;
}

void Player_SetCarSpeed(float s)
{
	g_CarSpeed = s;
}

int Player_GetSlowNumber()
{
	return g_SlowTimeNumber;
}

void Player_SetPosition(const DirectX::XMFLOAT3& position)
{
	g_PlayerPosition = position;
}

static float GetLeftStickX()
{
	XINPUT_STATE pad{};
	if (XInputGetState(0, &pad) != ERROR_SUCCESS)
		return 0.0f;

	float lx = pad.Gamepad.sThumbLX / 32767.0f;

	const float DEADZONE = 0.2f;
	if (fabs(lx) < DEADZONE)
		return 0.0f;

	return lx;
}