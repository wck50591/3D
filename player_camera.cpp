// ========================================================================
//    Name : プレイヤーカメラ制御(player_camera.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/10/31
// ========================================================================
#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "player.h"
#include "direct3d.h"
#include <random>

static XMFLOAT3 g_PlayerCameraPosition = { 0.0f,0.0f,0.0f };
static XMFLOAT3 g_PlayerCameraFront = { 0.0f,0.0f,1.0f };
static XMFLOAT3 g_PlayerCameraTargetPosition = { 0.0f,0.0f,0.0f };
static XMFLOAT4X4 g_PlayerCameraMatrix = {};
static XMFLOAT4X4 g_PlayerCameraPerspectiveMatrix = {};

static float g_CameraShake = 0.0f;
static double g_CameraShakeTime = 0.0;
static bool g_CameraStartShake = false;

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

void PlayerCamera_Initialize()
{

}

void PlayerCamera_Finalize()
{

}

void PlayerCamera_Update(double)
{

	// ここは “セットされたカメラ位置” を使う
	XMVECTOR position = XMLoadFloat3(&g_PlayerCameraPosition);
	//position += {0.0f, 2.0f, -3.0f};

	// ターゲット（プレイヤー）を見る
	XMVECTOR target = XMLoadFloat3(&g_PlayerCameraTargetPosition);

	XMVECTOR front = XMVector3Normalize(target - position);
	XMStoreFloat3(&g_PlayerCameraFront, front);


	// camera shake
	float s = dist(mt);

	XMVECTOR right = XMVector3Normalize(XMVector3Cross({ 0.0f,1.0f,0.0f }, front));

	target += right * g_CameraShake * s;
	position += right * g_CameraShake * s;

	s = dist(mt);
	target = XMVectorSetY(target, XMVectorGetY(target) + static_cast<float>(g_CameraShake * s));
	position = XMVectorSetY(position, XMVectorGetY(position) + static_cast<float>(g_CameraShake * s));

	g_CameraShake *= 0.9f;


	// ビュー座標変換行列の作成
	XMMATRIX mtxView = XMMatrixLookAtLH(
		position, // カメラの座標
		target, // 視点
		{0.0f,1.0f,0.0f}  // 上向き
	);
	
	//カメラ行列を保存
	XMStoreFloat4x4(&g_PlayerCameraMatrix, mtxView);

	// パースペクティブ行列の作成
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 50.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(
		1.0f, aspectRatio, nearZ, farZ
	);

	//パースペクティブ行列の保存
	XMStoreFloat4x4(&g_PlayerCameraPerspectiveMatrix,mtxPerspective);
}

const DirectX::XMFLOAT3& PlayerCamera_GetFront()
{
	return g_PlayerCameraFront;
}

const DirectX::XMFLOAT3& PlayerCamera_GetPosition()
{
	return g_PlayerCameraPosition;
}

void PlayerCamera_SetPosition(const DirectX::XMFLOAT3& p)
{
	g_PlayerCameraPosition = p;
}

const DirectX::XMFLOAT3& PlayerCamera_GetTargetPosition()
{
	return g_PlayerCameraTargetPosition;
}

void PlayerCamera_SetTargetPosition(const DirectX::XMFLOAT3& p)
{
	g_PlayerCameraTargetPosition = p;
}

const DirectX::XMFLOAT4X4& PlayerCamera_GetViewMatrix()
{
	return g_PlayerCameraMatrix;
}

const DirectX::XMFLOAT4X4& PlayerCamera_GetPerspectiveMatrix()
{
	return g_PlayerCameraPerspectiveMatrix;
}

Ray PlayerCamera_GetRay()
{
	return { g_PlayerCameraPosition,  g_PlayerCameraFront };
}

void PlayerCamera_Shake(float shake)
{
	g_CameraShake = shake;
	g_CameraShakeTime = 0.0;
}
