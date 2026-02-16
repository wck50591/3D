
// ========================================================================
//    Name : crosshair (crosshair.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/12/17
// ========================================================================

#ifndef CROSSHAIR_H
#define CROSSHAIR_H

static constexpr int MAX_TEXTURE = 8;

struct ICrosshairRenderer {
	virtual int LoadTexture(const wchar_t* path, bool IsMipMap) = 0;
	virtual int TextureWidth(int texId) const = 0;
	virtual int TextureHeight(int texId) const = 0;

	virtual void DrawSprite(int texId, float x, float y) = 0;
	virtual void DrawSprite(int texId, float x, float y, float w, float h) = 0;

	virtual ~ICrosshairRenderer() = default;
};

class Crosshair {
public:
	explicit Crosshair(ICrosshairRenderer& r);
	~Crosshair();

	void Initialize();
	void Finalize();
	void Update(int mouse_x, int mouse_y, int screen_w, int screen_h);

	// use GetTexture()!!!
	void DrawCenterScale(int texId, float scale);

	void SetVisible(bool visible);

	bool SetTexture(const wchar_t* path, bool IsMipMap);
	bool SetTexture(int slot, const wchar_t* path, bool IsMipMap);
	int GetTexture(int texId) const;

private:
	ICrosshairRenderer* m_r = nullptr;

	int m_x = 0;
	int m_y = 0;
	bool m_visible = true;

	int m_texture[MAX_TEXTURE];
};

#endif // !CROSSHAIR_H