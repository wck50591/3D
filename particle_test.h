
// ========================================================================
//    Name : particle_test (particle_test.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/02/04
// ========================================================================
#ifndef PARTICLE_TEST_H
#define PARTICLE_TEST_H

#include "particle.h"
#include "texture.h"
#include <random>

class NormalParticle : public Particle
{
private:
	int m_texture_id{-1};
	float m_scale{ 1.0f };
	float m_alpha{ 0.0f };

public:
	NormalParticle(int texture_id, const DirectX::XMVECTOR& position
		, const DirectX::XMVECTOR& velocity, double lift_time)
		: m_texture_id(texture_id), Particle(position, velocity, lift_time) {
	}

	void Update(double elapsed_time) override;
	void Draw() const override;
};

class NormalEmitter : public Emitter
{
private:
	int m_texture_id{-1};
	std::mt19937 m_mt;
	std::uniform_real_distribution<float> m_dir_dist{ -DirectX::XM_PI, DirectX::XM_PI };
	std::uniform_real_distribution<float> m_length_dist{ 0.1f, 2.0f };
	std::uniform_real_distribution<float> m_speed_dist{ 0.05f, 0.1f };
	std::uniform_real_distribution<float> m_lifetime_dist{ 0.5f, 1.0f };

public:
	NormalEmitter(size_t capacity, const DirectX::XMVECTOR& position, double particles_per_second
		, bool is_emmit = false)
		:Emitter(capacity, position, particles_per_second, is_emmit)
		, m_texture_id(Texture_Load(L"resource/texture/effect000.jpg",true))
	{
	}
protected:
	Particle* CreateParticle() override;
};
#endif // !PARTICLE_TEST_H
