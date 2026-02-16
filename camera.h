
// ========================================================================
//    Name : ÉJÉÅÉâêßå‰(camera.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/11
// ========================================================================


#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

void Camera_Initialize();
void Camera_Initialize(
	const DirectX::XMFLOAT3& position, 
	const DirectX::XMFLOAT3& front,
	const DirectX::XMFLOAT3& right
);
void Camera_Finalize();
void Camera_Update(double elapsed_time);

const DirectX::XMFLOAT4X4& Camera_GetMatrix();
const DirectX::XMFLOAT4X4& Camera_GetPerspectiveMatrix();

const DirectX::XMFLOAT3& Camera_GetPosition();
const DirectX::XMFLOAT3& Camera_GetFront();
const DirectX::XMFLOAT3& Camera_GetRight();
const DirectX::XMFLOAT3& Camera_GetUp();
float Camera_GetFov();

void Camera_SetMatrix(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);

void Camera_DebugDraw();
void Camera_SetPositionX(float x);
void Camera_SetPositionY(float y);
void Camera_SetPositionZ(float z);
void Camera_SetFov(float fov);
#endif // !CAMERA_H
