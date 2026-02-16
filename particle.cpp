
// ========================================================================
//    Name : particle (particle.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/02/04
// ========================================================================

#include "particle.h"
using namespace DirectX;

void Emitter::Update(double elapsed_time)
{
	m_accumulated_time += elapsed_time;

	const double sec_per_particle = 1.0 / m_particles_per_second;

	// ï¨èo
	while (m_accumulated_time >= sec_per_particle) {
		if (m_count >= m_capacity) break;
		if (m_is_emmit) {
			m_particles[m_count++] = CreateParticle();
		}
		m_accumulated_time -= sec_per_particle;
	}

	// çXêV
	for (int i = 0; i < m_count; i++) {
		m_particles[i]->Update(elapsed_time);
	}

	// éıñΩ
	for (int i = m_count -1; i >= 0; i--) {
		if (m_particles[i]->IsDestroy()) {
			delete m_particles[i];
			m_particles[i] = m_particles[--m_count];
		}
	}
}

void Emitter::Draw() const
{
	for (int i = 0; i < m_count; i++) {
		m_particles[i]->Draw();
	}
}
