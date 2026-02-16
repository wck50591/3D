
// ========================================================================
//    Name : Map(level)(map.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/10
// ========================================================================
#include "map.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "cube.h"
#include "texture.h"
#include "light.h"
#include "player_camera.h"
#include "player.h"
#include "meshfield.h"
#include "model.h"
#include "material.h"
#include "time.h"
#include <time.h>

static constexpr int MAP_OBJECT_MAX = 1024;

static constexpr int CHUNK_COUNT = 3;
static constexpr float CHUNK_LENGTH = 50.0f;

static float g_ChunkZ[CHUNK_COUNT] = {
	0.0f,
	CHUNK_LENGTH,
	CHUNK_LENGTH * 2.0f
};

static MapObject g_RuntimeObjects[MAP_OBJECT_MAX];
static int g_RuntimeCount = 0;

static const MapObject g_BaseObjects[] =
{
	{FIELD, { 0.0f, 0.0f, 0.0f}, {{-25.0f, -1.0f, -25.0f},{25.0f, 0.0f, 25.0f}}},

	{TREE, { 8.0f, 0.0f, 3.0f}},
	{TREE, { 6.0f, 0.0f, 4.0f}},
	{TREE, { -8.0f, 0.0f, -3.0f}},
	{TREE, { -6.0f, 0.0f, -4.0f}},
	{ROCK, { -8.0f, 0.0f, 0.0f}},
	{ROCK, { -8.0f, 0.0f, 5.0f}},
	{ROCK, { -6.0f, 0.0f, -2.0f}},
	{ROCK, { 6.0f, 0.0f, -3.0f}},
};

static int g_CubeTexIds[2] = { -1, -1 };
static MODEL* roadblock = nullptr;
static MODEL* tree = nullptr;
static MODEL* rock = nullptr;
static int pointLightIndex = 0;

static void Map_AddObject(const MapObject& obj);

void Map_Initialize()
{
	srand((unsigned int)time(nullptr));

	g_RuntimeCount = 0;

	//g_CubeTexIds[0] = Texture_Load(L"resource/texture/number_512x512.png", TRUE);
	roadblock = ModelLoad("resource/model/roadblock.fbx", 0.2f, false);
	tree = ModelLoad("resource/model/tree_2.fbx", 0.5f, false);
	rock = ModelLoad("resource/model/rock.fbx", 1.0f, false);

	static float lanes[] = { -1.0f, 0.0f, 1.0f };

	const float playerStartZ = -15.0f;

	float baseZ = floorf(playerStartZ / CHUNK_LENGTH) * CHUNK_LENGTH;
	float playerLocalZ = playerStartZ - baseZ;

	const float safe_behind = 2.0f;
	const float safe_ahead = 3.0f;
	float safeMin = playerLocalZ - safe_behind;
	float safeMax = playerLocalZ + safe_ahead;

	float zLocal = 10.0f;
	const float end_local_z = 48.0f;
	const float step_min = 1.0f;
	const float step_max = 6.0f;

	int lastLane = -1;

	while (zLocal <= end_local_z)
	{
		if (zLocal >= safeMin && zLocal <= safeMax)
		{
			zLocal = safeMax + 0.5f;
			continue;
		}

		int lane;
		do { lane = rand() % 3; } while (lane == lastLane);
		lastLane = lane;

		Map_AddObject({ ROADBLOCK, { lanes[lane], 0.0f, zLocal } });

		float t = (float)rand() / (float)RAND_MAX;
		zLocal += step_min + t * (step_max - step_min);
	}

	float z = 0.0f;

	while (z < CHUNK_LENGTH)
	{
		// spacing along Z
		z += 2.0f + (rand() % 2);   // 2 ~ 5 units

		// chance to spawn (not every step)
		if (rand() % 2 != 0) continue;

		float x = (rand() % 2) ? 4.0f : -4.0f;

		int type = (rand() % 2 == 0) ? TREE : ROCK;

		Map_AddObject({ type, { x, 0.0f, z } });

		Map_AddObject({ TREE, {  2.3f, 0.0f, z } });
		Map_AddObject({ TREE, { -2.3f, 0.0f, z } });
		Map_AddObject({ TREE, {  3.3f, 0.0f, z + 2.0f } });
		Map_AddObject({ TREE, { -3.3f, 0.0f, z + 2.0f } });
		Map_AddObject({ TREE, {  4.3f, 0.0f, z + 1.0f} });
		Map_AddObject({ TREE, { -4.3f, 0.0f, z + 1.0f} });
		Map_AddObject({ TREE, {  5.3f, 0.0f, z + 3.0f} });
		Map_AddObject({ TREE, { -5.3f, 0.0f, z + 3.0f} });
		Map_AddObject({ TREE, {  6.3f, 0.0f, z + 4.0f} });
		Map_AddObject({ TREE, { -6.3f, 0.0f, z + 4.0f} });
	}


	for (const MapObject& b : g_BaseObjects) {
		Map_AddObject(b);
	}

	for (float z = 25.0f; z >= -25.0f; z -= 2.0f)
	{
		Map_AddObject({ IV, {  2.0f, 0.5f, z }});
		Map_AddObject({ IV, { -2.0f, 0.5f, z }});
	}

	for (int i = 0; i < g_RuntimeCount; ++i)
	{
		MapObject& o = g_RuntimeObjects[i];

		if (o.TypeId == ROADBLOCK)
		{
			o.Aabb = Model_GetAABB(roadblock, o.Position);
			o.Aabb.min.y -= 0.5f;
			o.Aabb.max.z += 0.2f;
		}
		else if (o.TypeId == IV)
		{
			o.Aabb = Cube_GetAABB(o.Position);
		}
	}
}

void Map_Finalize()
{
	ModelRelease(rock);
	ModelRelease(tree);
	ModelRelease(roadblock);
}

void Map_Draw()
{
    Meshfield_Draw();

    XMFLOAT3 p = Player_GetPosition();
    float baseZ = floorf(p.z / CHUNK_LENGTH) * CHUNK_LENGTH;

    for (int i = -1; i <= 1; ++i)
    {
        float cz = baseZ + i * CHUNK_LENGTH;

		for (int j = 0; j < g_RuntimeCount; ++j)
		{
			const MapObject& o = g_RuntimeObjects[j];

            if (o.TypeId == FIELD) continue;

            XMMATRIX mtxWorld = XMMatrixTranslation(
                o.Position.x,
                o.Position.y,
                o.Position.z + cz
            );

			AABB worldAabb = o.Aabb;
			worldAabb.min.z += cz;
			worldAabb.max.z += cz;

#if defined(DEBUG) || defined(_DEBUG)
			Collision_DebugDraw(worldAabb);
#endif
			switch (o.TypeId)
			{
			case ROADBLOCK:
			{
				float t = (float)Time_GetTotal();
				float glow = 0.5f + 0.5f * sinf(t * 3.0f);

				Material_SetEmissive({ 0.8f, 0.4f, 0.1f, glow });
				ModelDraw(roadblock, mtxWorld);
				Material_SetEmissive({ 0,0,0,0 });
				break;
			}
            case TREE:    ModelDraw(tree, mtxWorld); break;
            case ROCK:    ModelDraw(rock, mtxWorld); break;
            }
        }
    }

}

int Map_GetObjectsCount()
{
	return g_RuntimeCount;
}

const MapObject* Map_GetObject(int index)
{
	return &g_RuntimeObjects[index];
}

static void Map_AddObject(const MapObject& obj)
{
	if (g_RuntimeCount >= MAP_OBJECT_MAX) return;
	g_RuntimeObjects[g_RuntimeCount++] = obj;
}
