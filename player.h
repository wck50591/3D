
// ========================================================================
//    Name : Player(player.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/10/31
// ========================================================================
#ifndef PLAYER_H
#define PLAYER_H

#include <directXMath.h>
#include "collision.h"

void Player_Initialize(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& front);
void Player_Finalize();
void Player_Update(double elapsed_time);
void Player_Draw();

const DirectX::XMFLOAT3& Player_GetPosition();
void Player_SetPosition(const DirectX::XMFLOAT3& position);
const DirectX::XMFLOAT3& Player_GetFront();
AABB Player_GetAABB();
AABB Player_ConvertPositionToAABB(const DirectX::XMVECTOR& position);
bool Player_IsGameover();
bool Player_IsStart();
void Player_SetStart(bool b);
float Player_GetCarSpeed();
void Player_SetCarSpeed(float s);
int Player_GetSlowNumber();

#endif // !PLAYER_H
