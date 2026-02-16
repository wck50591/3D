
// ========================================================================
//    Name : カメラ制御(camera.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/11
// ========================================================================

#include "camera.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "key_logger.h"
#include "keyboard.h"
#include "debug_text.h"
#include <sstream>

static XMFLOAT3 g_CameraPosition = { 0.0f, 5.0f, -5.0f };

static XMFLOAT3 g_CameraFront = { 0.0f,0.0f,1.0f };
static XMFLOAT3 g_CameraUp = { 0.0f,1.0f,0.0f };
static XMFLOAT3 g_CameraRight = { 1.0f,0.0f,0.0f };

static constexpr float CAMERA_MOVE_SPEED = 6.0f;
static constexpr float CAMERA_ROTATION_SPEED = XMConvertToRadians(60);

static XMFLOAT4X4 g_CameraMatrix;
static XMFLOAT4X4 g_PerspectiveMatrix;
static float g_Fov = XMConvertToRadians(60);

static hal::DebugText* g_pDT = nullptr;

static ID3D11Buffer* g_pVSConstantBuffer1 = nullptr;	//定数バッファb1 view
static ID3D11Buffer* g_pVSConstantBuffer2 = nullptr;	//定数バッファb2 projection

void Camera_Initialize()
{
	g_CameraPosition = {0.0f,2.0f,-5.0f};
	g_CameraFront = { 0.0f,0.0f,1.0f };
	g_CameraUp = { 0.0f,1.0f,0.0f };
	g_CameraRight = { 1.0f,0.0f,0.0f };
	g_Fov = XMConvertToRadians(60);

	XMStoreFloat4x4(&g_CameraMatrix,XMMatrixIdentity());
	XMStoreFloat4x4(&g_PerspectiveMatrix, XMMatrixIdentity());

	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer1);
	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &g_pVSConstantBuffer2);

#if defined(DEBUG) || defined(_DEBUG)
	g_pDT = new hal::DebugText(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"resource/texture/consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 30.0f, // offset
		0, 0,
		0.0f, 16.0f);

#endif
}

void Camera_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front, const DirectX::XMFLOAT3& right)
{

	Camera_Initialize();

	g_CameraPosition = position;
	XMVECTOR f = XMVector3Normalize(XMLoadFloat3(&front));
	XMVECTOR r = XMVector3Normalize(XMLoadFloat3(&right) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(f,r));

	XMStoreFloat3(&g_CameraFront,f);
	XMStoreFloat3(&g_CameraRight, r);
	XMStoreFloat3(&g_CameraUp, u);
	/*g_CameraFront = front;
	g_CameraUp = up;
	g_CameraRight = right;*/
	
}

void Camera_Finalize()
{
	SAFE_RELEASE(g_pVSConstantBuffer2);
	SAFE_RELEASE(g_pVSConstantBuffer1);
	delete g_pDT;
}

void Camera_Update(double elapsed_time)
{
	XMVECTOR front = XMLoadFloat3(&g_CameraFront);
	XMVECTOR up = XMLoadFloat3(&g_CameraUp);
	XMVECTOR right = XMLoadFloat3(&g_CameraRight);
	XMVECTOR position = XMLoadFloat3(&g_CameraPosition);

	//上向く
	if (KeyLogger_IsPressed(KK_I)) {
		XMMATRIX rotation = XMMatrixRotationAxis(right, -CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		//up = XMVector3Cross(front, right);
		up = XMVector3Normalize(XMVector3Cross(front, right));
	}
	//下向く
	if (KeyLogger_IsPressed(KK_K)) {
		XMMATRIX rotation = XMMatrixRotationAxis(right, CAMERA_ROTATION_SPEED * (float)elapsed_time);
		front = XMVector3TransformNormal(front,rotation);
		front = XMVector3Normalize(front);
		//up = XMVector3Cross(front,right);
		up = XMVector3Normalize(XMVector3Cross(front, right));
	}
	//左向く
	if (KeyLogger_IsPressed(KK_J)) {
		XMMATRIX rotation = XMMatrixRotationY(-CAMERA_ROTATION_SPEED * (float)elapsed_time);
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation));

		//XMMATRIX rotation = XMMatrixRotationAxis(up, -CAMERA_ROTATION_SPEED * elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		//right = XMVector3Cross(up, front);
		right = XMVector3Normalize(XMVector3Cross(up, front) * XMVECTOR { 1.0f, 0.0f, 1.0f });
	}
	//右向く
	if (KeyLogger_IsPressed(KK_L)) {
		XMMATRIX rotation = XMMatrixRotationY(CAMERA_ROTATION_SPEED * (float)elapsed_time);
		up = XMVector3Normalize(XMVector3TransformNormal(up, rotation));

		//XMMATRIX rotation = XMMatrixRotationAxis(up, CAMERA_ROTATION_SPEED * elapsed_time);
		front = XMVector3TransformNormal(front, rotation);
		front = XMVector3Normalize(front);
		//right = XMVector3Cross(up,front);
		//right = XMVector3Normalize(XMVector3Cross(up, front));
		right = XMVector3Normalize(XMVector3Cross(up, front) * XMVECTOR {1.0f,0.0f,1.0f});
	}
	
	//if (KeyLogger_IsPressed(KK_E)) {
	//	XMMATRIX rotation = XMMatrixRotationAxis(front, -CAMERA_ROTATION_SPEED * (float)elapsed_time);
	//	right = XMVector3TransformNormal(right, rotation);
	//	right = XMVector3Normalize(right);
	//	//up = XMVector3Cross(front,right);
	//	up = XMVector3Normalize(XMVector3Cross(front, right));
	//}
	//if (KeyLogger_IsPressed(KK_Q)) {
	//	XMMATRIX rotation = XMMatrixRotationAxis(front, CAMERA_ROTATION_SPEED * (float)elapsed_time);
	//	right = XMVector3TransformNormal(right, rotation);
	//	right = XMVector3Normalize(right);
	//	//up = XMVector3Cross(front, right);
	//	up = XMVector3Normalize(XMVector3Cross(front, right));
	//}
	
	//移動
	if (KeyLogger_IsPressed(KK_UP)) {
		//position += front * CAMERA_MOVE_SPEED * elapsed_time;
		position += XMVector3Normalize(front * XMVECTOR {1.0f, 0.0f, 1.0f}) * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_DOWN)) {
		//position += -front * CAMERA_MOVE_SPEED * elapsed_time;
		position += XMVector3Normalize(-front * XMVECTOR { 1.0f, 0.0f, 1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_LEFT)) {
		//position += -right * CAMERA_MOVE_SPEED * (float)elapsed_time;
		position += XMVector3Normalize(-right * XMVECTOR{ 1.0f, 0.0f, 1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_RIGHT)) {
		//position += right * CAMERA_MOVE_SPEED * (float)elapsed_time;
		position += XMVector3Normalize(right * XMVECTOR{ 1.0f, 0.0f, 1.0f }) * CAMERA_MOVE_SPEED * (float)elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_Q)) {
		//position += up * CAMERA_MOVE_SPEED * elapsed_time;
		position += XMVECTOR {0.0f, 1.0f, 0.0f} * CAMERA_MOVE_SPEED * (float)elapsed_time; // fps
	}
	if (KeyLogger_IsPressed(KK_E)) {
		//position += -up* CAMERA_MOVE_SPEED * elapsed_time;
		position += XMVECTOR{ 0.0f, -1.0f, 0.0f } *CAMERA_MOVE_SPEED * (float)elapsed_time;// fps
	}
	if (KeyLogger_IsPressed(KK_Z)) {
		g_Fov -= XMConvertToRadians(10) * elapsed_time;
	}
	if (KeyLogger_IsPressed(KK_C)) {
		g_Fov += XMConvertToRadians(10) * elapsed_time;
	}

	//各種更新結果を保存
	XMStoreFloat3(&g_CameraPosition, position);
	XMStoreFloat3(&g_CameraFront, front);
	XMStoreFloat3(&g_CameraUp, up);
	XMStoreFloat3(&g_CameraRight, right);


	// ビュー座標変換行列の作成
	XMMATRIX mtxView = XMMatrixLookAtLH(
		position, // カメラの座標
		position + front, // 視点
		up  // 上向き
	);

	// ビュー座標変換行列の保存
	XMStoreFloat4x4(&g_CameraMatrix, mtxView);

	// パースペクティブ行列の作成
	float aspectRatio = (float)Direct3D_GetBackBufferWidth() / Direct3D_GetBackBufferHeight();
	float nearZ = 0.1f;
	float farZ = 100.0f;
	XMMATRIX mtxPerspective = XMMatrixPerspectiveFovLH(
		g_Fov, aspectRatio, nearZ, farZ
	);

	// パースペクティブ行列の保存
	XMStoreFloat4x4(&g_PerspectiveMatrix, mtxPerspective);
}

const DirectX::XMFLOAT4X4& Camera_GetMatrix()
{
	return g_CameraMatrix;
}

const DirectX::XMFLOAT4X4& Camera_GetPerspectiveMatrix()
{
	return g_PerspectiveMatrix;
}

const DirectX::XMFLOAT3& Camera_GetPosition()
{
	return g_CameraPosition;
}

const DirectX::XMFLOAT3& Camera_GetFront()
{
	return g_CameraFront;
}
const DirectX::XMFLOAT3& Camera_GetRight()
{
	return g_CameraRight;
}
const DirectX::XMFLOAT3& Camera_GetUp()
{
	return g_CameraUp;
}
float Camera_GetFov()
{
	//return 0.0f;
	return g_Fov;
}

void Camera_SetMatrix(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	// 定数バッファに行列をセット
	XMFLOAT4X4 v, p;
	XMStoreFloat4x4(&v, XMMatrixTranspose(view));
	XMStoreFloat4x4(&p, XMMatrixTranspose(projection));

	Direct3D_GetContext()->UpdateSubresource(g_pVSConstantBuffer1, 0, nullptr, &v, 0, 0);
	Direct3D_GetContext()->VSSetConstantBuffers(1, 1, &g_pVSConstantBuffer1);

	Direct3D_GetContext()->UpdateSubresource(g_pVSConstantBuffer2, 0, nullptr, &p, 0, 0);
	Direct3D_GetContext()->VSSetConstantBuffers(2, 1, &g_pVSConstantBuffer2);

}

void Camera_DebugDraw()
{

#if defined(DEBUG) || defined(_DEBUG)
	std::stringstream ss;

	ss << "Camera Position	: X = " << g_CameraPosition.x;
	ss << " Y = " << g_CameraPosition.y;
	ss << " Z = " << g_CameraPosition.z << std::endl;

	ss << "Camera Front		: X = " << g_CameraFront.x;
	ss << " Y = " << g_CameraFront.y;
	ss << " Z = " << g_CameraFront.z << std::endl;

	ss << "Camera Right		: X = " << g_CameraRight.x;
	ss << " Y = " << g_CameraRight.y;
	ss << " Z = " << g_CameraRight.z << std::endl;

	ss << "Camera Up		: X = " << g_CameraUp.x;
	ss << " Y = " << g_CameraUp.y;
	ss << " Z = " << g_CameraUp.z << std::endl;

	ss << "Camera Fov		: " << g_Fov << std::endl;

	g_pDT->SetText(ss.str().c_str(), {0.0f,1.0f,0.0,1.0f});
	g_pDT->Draw();
	g_pDT->Clear();

#endif
}

void Camera_SetPositionX(float x)
{
	g_CameraPosition.x = x;
}

void Camera_SetPositionY(float y)
{
	g_CameraPosition.y = y;
}

void Camera_SetPositionZ(float z)
{
	g_CameraPosition.z = z;
}

float Camera_GetFrontX()
{
	return g_CameraFront.x;
}

void Camera_SetFov(float fov)
{	
	g_Fov = XMConvertToRadians(fov);
}


