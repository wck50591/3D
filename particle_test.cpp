
// ========================================================================
//    Name : particle_test (particle_test.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/02/04
// ========================================================================

#include "particle_test.h"
#include "billboard.h"
using namespace DirectX;

void NormalParticle::Update(double elapsed_time)
{
	float ratio = static_cast<float>(std::min(GetAccumulatedTime() / GetLiftTime(), 1.0));
	m_scale = (1.0f - ratio) * 0.5f;
	m_alpha = (1.0f - ratio);

	AddPosition(GetVelocity() * static_cast<float>(elapsed_time));
	//AddVelocity(XMVECTOR{0.0f, -3.0f, 0.0f} * elapsed_time);
	Particle::Update(elapsed_time);
}

void NormalParticle::Draw() const
{
	XMFLOAT3 position{};
	XMStoreFloat3(&position, GetPosition());
	Billboard_Draw(m_texture_id, position, { m_scale , m_scale }, XMFLOAT4{0.5f, 0.3f, 0.2f, m_alpha});
	Billboard_Draw(m_texture_id, position, { m_scale * 0.5f , m_scale * 0.5f }, XMFLOAT4{1.0f, 0.2f, 0.2f, m_alpha});
}

Particle* NormalEmitter::CreateParticle()
{
	//std::uniform_real_distribution<float> m_dir_dist{ -XM_PI, XM_PI };
	//std::uniform_real_distribution<float> m_length_dist{ 0.1f, 2.0f };
	//
	//std::uniform_real_distribution<float> m_speed_dist{ 0.05f, 0.1f };
	//std::uniform_real_distribution<float> m_lifetime_dist{ 0.5f, 1.0f };

	float angle = m_dir_dist(m_mt);
	//float ey = XMVectorGetY(GetPosition());
	XMVECTOR c{ cosf(angle), 0.0f, sinf(angle) };
	c *= m_length_dist(m_mt);
	c += GetPosition();
	//c += {0.0f, 10.0f, 0.0f};
	//XMVECTOR direction{ c - GetPosition() };

	//std::uniform_real_distribution<float> dist{ 0.0f, XM_PI };
	//std::uniform_real_distribution<float> dist{ XMConvertToRadians(80), XMConvertToRadians(100) };
	//float y = dist(m_mt);
	//XMVECTOR direction{cosf(angle), sinf(XMConvertToRadians(90)), sinf(angle) };
	
	//direction = XMVector3Normalize(direction);
	//XMVECTOR velocity{ direction * m_speed_dist(m_mt) };
	XMVECTOR velocity{ 0.0f, 1.0f, 0.0f };

	//return new NormalParticle(m_texture_id, GetPosition(), velocity, m_lifetime_dist(m_mt));
	return new NormalParticle(m_texture_id, c, velocity, m_lifetime_dist(m_mt));
}
