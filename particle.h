
// ========================================================================
//    Name : particle (particle.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/02/04
// ========================================================================

#ifndef PARTICLE_H
#define PARTICLE_H

#include <DirectXMath.h>
//using DirectX::operator+=;

// インスタンシング
// ジオメトリシェーダー
// GPUパーティクル

class Particle {
private:
	DirectX::XMVECTOR m_position{};
	DirectX::XMVECTOR m_velocity{};
	// float m_mass;
	double m_life_time{};
	double m_accumulated_time{};
	
public:
	Particle(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& velocity
		, double lift_time)
		: m_position(position), m_velocity(velocity), m_life_time(lift_time) {
	}
	virtual ~Particle() = default;

	virtual bool IsDestroy() const { return m_life_time <= 0.0; };

	virtual void Update(double elpased_time) {
		m_accumulated_time += elpased_time;
		if (m_accumulated_time > m_life_time) { Destroy(); };
	}

	virtual void Draw() const = 0;

protected:
	virtual void Destroy() {
		m_life_time = 0.0;
	}

	const DirectX::XMVECTOR& GetPosition() const { return m_position; }
	void SetPosition(const DirectX::XMVECTOR& position) { m_position = position; }

	const DirectX::XMVECTOR& GetVelocity() const { return m_velocity; }
	void SetVelocity(const DirectX::XMVECTOR& velocity) { m_velocity = velocity; }

	void AddPosition(const DirectX::XMVECTOR& v) { 
		m_position = DirectX::XMVectorAdd(m_position, v);
	}

	void AddVelocity(const DirectX::XMVECTOR& v) {
		m_velocity = DirectX::XMVectorAdd(m_velocity, v);
	}

	double GetAccumulatedTime() const {
		return m_accumulated_time;
	}

	double GetLiftTime() const {
		return m_life_time;
	}
};

class Emitter // 噴出機
{
private:
	DirectX::XMVECTOR m_position{};
	double m_particles_per_second{};
	double m_accumulated_time = 0.0;
	bool m_is_emmit{};
	size_t m_capacity{}; // パーティクル管理最大量
	size_t m_count{}; // 現在のパーティクル数
	Particle** m_particles{};

protected:
	virtual Particle* CreateParticle() = 0;

	const DirectX::XMVECTOR& GetPosition() const { return m_position; };

	double GetParticlesPerSecond() const { return m_particles_per_second; };
	void SetParticlesPerSecond(double particles_per_second) { m_particles_per_second = particles_per_second; };

	double GetAccumulatedTime() const { return m_accumulated_time; };

public:
	Emitter(size_t capacity, const DirectX::XMVECTOR& position, double particles_per_second
		, bool is_emmit = false)
		: m_capacity(capacity)
		, m_position(position)
		, m_particles_per_second(particles_per_second)
		, m_is_emmit(is_emmit)
	{
		m_particles = new Particle*[m_capacity];
	}

	virtual ~Emitter() { 
		delete[] m_particles;
	};

	virtual void Update(double elapsed_time);
	virtual void Draw() const;

	bool IsEmmit() const { return m_is_emmit; };
	void Emmit(bool isEmmit) { m_is_emmit = isEmmit; };

	void SetPosition(DirectX::XMVECTOR& position) { m_position = position; };

};

#endif // !PARTICLE_H
