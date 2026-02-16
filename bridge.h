
// ========================================================================
//    Name :  bridge (bridge.h)
//
// connect your own file function to the added file's function
// 
//                                                    Author : wai chunkit
//                                                    Date   : 2025/12/21
// ========================================================================

#ifndef BRIDGE_H
#define BRIDGE_H

#include "crosshair.h"
#include "texture.h"
#include "sprite.h"

struct CrosshairRenderer : public ICrosshairRenderer {

	int LoadTexture(const wchar_t* path, bool IsMipMap) override {
		return Texture_Load(path, IsMipMap);
	}

	int TextureWidth(int texId) const override{
		return Texture_Width(texId);
	}

	int TextureHeight(int texId) const override {
		return Texture_Height(texId);
	}

	void DrawSprite(int texId, float x, float y) {
		Sprite_Draw(texId,x,y);
	}

	void DrawSprite(int texId, float x, float y, float w, float h) {
		Sprite_Draw(texId, x, y, w, h);
	}
};

#endif // !BRIDGE_H
