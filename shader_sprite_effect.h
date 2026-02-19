
// ========================================================================
//    Name : Shader for drawing sprite with effect (shader_sprite_effect.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/02/18
// ========================================================================
#ifndef SHADER_SPRITE_EFFECT_H
#define SHADER_SPRITE_EFFECT_H

#include <d3d11.h>
#include <DirectXMath.h>

struct SpriteFX
{
	float time;
	float intensity;
	float sliceHeight;
	float dum;
};

class ShaderSpriteEffect {
private:

	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	ID3D11Buffer* m_pVSConstantBuffer0 = nullptr;
	ID3D11Buffer* m_pVSConstantBuffer1 = nullptr;
	ID3D11Buffer* m_pPSConstantBuffer0 = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;

	// 注意！初期化で外部から設定されるもの。Release不要。
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	ShaderSpriteEffect() = default;
	~ShaderSpriteEffect() = default;

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Finalize();

	void SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
	void SetWorldMatrix(const DirectX::XMMATRIX& matrix);
	void SetGlitch(float time, float intensity, float sliceHeight);

	void Begin();
};

#endif // !SHADER_SPRITE_EFFECT_H
