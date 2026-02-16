
// ========================================================================
//    Name : crosshair (crosshair.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/12/17
// ========================================================================

#include "crosshair.h"
#include "my_math.h"

Crosshair::Crosshair(ICrosshairRenderer& r)
{
	m_r = &r;
}

Crosshair::~Crosshair()
{
	Finalize();
}

void Crosshair::Initialize()
{
	if (!m_r) return;

	for (int i = 0; i < MAX_TEXTURE;i++) {
		m_texture[i] = -1;
	}

	m_visible = true;
}

void Crosshair::Finalize()
{
	for (int i = 0; i < MAX_TEXTURE;i++) {
		if (m_texture[i] != -1) {
			m_texture[i] = -1;
		}
	}
}

void Crosshair::Update(int mouse_x, int mouse_y, int screen_w, int screen_h)
{
	m_x = clamp<int>(mouse_x, 0, screen_w - 1);
	m_y = clamp<int>(mouse_y, 0, screen_h - 1);

}

void Crosshair::DrawCenterScale(int texId, float scale)
{
	if (texId < 0 || texId >= MAX_TEXTURE) return;

	m_r->DrawSprite(texId,
		(float)m_x - m_r->TextureWidth(texId) * 0.5f * scale,
		(float)m_y - m_r->TextureHeight(texId) * 0.5f * scale,
		m_r->TextureWidth(texId) * scale,
		m_r->TextureHeight(texId) * scale
	);
}

void Crosshair::SetVisible(bool visible)
{
	m_visible = visible;
}

bool Crosshair::SetTexture(const wchar_t* path, bool IsMipMap)
{
	if (!m_r) return false;

	for (int i = 0; i < MAX_TEXTURE;i++) {
		if (m_texture[i] == -1) {
			m_texture[i] = m_r->LoadTexture(path, IsMipMap);
			return true;
		};
	}

	return false;
}

bool Crosshair::SetTexture(int slot, const wchar_t* path, bool IsMipMap)
{
	if (!m_r) return false;
	if (slot < 0 || slot > MAX_TEXTURE) return false;

	m_texture[slot] = m_r->LoadTexture(path, IsMipMap);

	return false;
}

int Crosshair::GetTexture(int texId) const
{
	if (texId < 0 || texId >= MAX_TEXTURE) return -1;

	return m_texture[texId];
}