
// ========================================================================
//    Name : bullet(bullet.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/12
// ========================================================================

#ifndef BULLET_H
#define BULLET_H

#include <directXMath.h>
#include "collision.h"

void Bullet_Initialize();
void Bullet_Finalize();
void Bullet_Update(double elapsed_time);
void Bullet_Draw();

void Bullet_Create(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity);
void Bullet_Destroy(int index);

int Bullet_GetBulletsCount();
AABB Bullet_GetAABB(int index);
const DirectX::XMFLOAT3& Bullet_GetPosition(int index);

#endif //BULLET_H