
// ========================================================================
//    Name : プレイヤーカメラ制御(player_camera.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/10/31
// ========================================================================

#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include <DirectXMath.h>
#include "collision.h"

void PlayerCamera_Initialize();
void PlayerCamera_Finalize();
void PlayerCamera_Update(double elapsed_time);

const DirectX::XMFLOAT3& PlayerCamera_GetFront();
const DirectX::XMFLOAT3& PlayerCamera_GetPosition();
void PlayerCamera_SetPosition(const DirectX::XMFLOAT3& p);
const DirectX::XMFLOAT3& PlayerCamera_GetTargetPosition();
void PlayerCamera_SetTargetPosition(const DirectX::XMFLOAT3& p);

const DirectX::XMFLOAT4X4& PlayerCamera_GetViewMatrix();
const DirectX::XMFLOAT4X4& PlayerCamera_GetPerspectiveMatrix();

Ray PlayerCamera_GetRay();
void PlayerCamera_Shake(float shake);

#endif // !PLAYER_CAMERA_H
