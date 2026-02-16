
// ========================================================================
//    Name : Map(level)(map.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/10
// ========================================================================

#ifndef MAP_H
#define MAP_H

#include <DirectXMath.h>
#include "Collision.h"

void Map_Initialize();
void Map_Finalize();

//void Map_Update(double elapsed_time);
void Map_Draw();

int Map_GetObjectsCount();

enum ObjectKind {
	FIELD,
	IV,
	ROADBLOCK,
	WOODBOX,
	TREE,
	ROCK,
};

struct MapObject {
	int TypeId;
	DirectX::XMFLOAT3 Position;
	AABB Aabb;
};

const MapObject* Map_GetObject(int index);

#endif // !MAP_H
