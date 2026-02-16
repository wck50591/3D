
// ========================================================================
//    Name : Post Effect (post_effect.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/29
// ========================================================================

#ifndef POST_EFFECT_H
#define POST_EFFECT_H

#include <d3d11.h>

bool PostEffect_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void PostEffect_Finalize();

void PostEffect_Clear();
void PostEffect_Begin();
void PostEffect_End();

void PostEffect_InverseColor(float strength);

#endif // !POST_EFFECT_H
