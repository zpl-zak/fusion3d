/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "level.h"
#include "../3DEngine.h"

#define NUM_TEX_EXP 4
#define NUM_TEXTURES ((int)pow(2, NUM_TEX_EXP))

Level::Level(Game* game, Entity* entity, std::string mapName, Vector3f spot, std::string matName)
{
	Bitmap level(mapName);
	level.FlipY();

	const int SPOT_W = spot.GetX();
	const int SPOT_H = spot.GetY();
	const int SPOT_L = spot.GetZ();

	IndexedModel map;
	{
		for (int i = 0; i < level.GetWidth(); i++)
		{
			for (int j = 0; j < level.GetHeight(); j++)
			{
				if ((level.GetPixel(i, j) & 0xFFFFFF) == 0)
					continue;

				int texX = ((level.GetPixel(i, j) & 0x00FF00) >> 8) / NUM_TEXTURES;
				int texY = texX % NUM_TEX_EXP;
				texX /= NUM_TEX_EXP;

				float XH = 1.0f - (float)texX / (float)NUM_TEX_EXP;
				float XL = XH - 1 / (float)NUM_TEX_EXP;
				float YL = 1.0f - (float)texY / (float)NUM_TEX_EXP;
				float YH = YL - 1 / (float)NUM_TEX_EXP;

				//Floor
				map.AddFace(map.GetPositions().size() + 2,
				            map.GetPositions().size() + 1,
				            map.GetPositions().size() + 0
				);

				map.AddFace(map.GetPositions().size() + 3,
				            map.GetPositions().size() + 2,
				            map.GetPositions().size() + 0
				);

				map.AddVertex(Vector3f(i * SPOT_W, 0, j * SPOT_L));
				map.AddTexCoord(Vector2f(XL, YL));
				map.AddVertex(Vector3f((i + 1) * SPOT_W, 0, j * SPOT_L));
				map.AddTexCoord(Vector2f(XH, YL));
				map.AddVertex(Vector3f((i + 1) * SPOT_W, 0, (j + 1) * SPOT_L));
				map.AddTexCoord(Vector2f(XH, YH));
				map.AddVertex(Vector3f(i * SPOT_W, 0, (j + 1) * SPOT_L));
				map.AddTexCoord(Vector2f(XL, YH));

				//Ceiling
				map.AddFace(map.GetPositions().size() + 0,
				            map.GetPositions().size() + 1,
				            map.GetPositions().size() + 2
				);

				map.AddFace(map.GetPositions().size() + 0,
				            map.GetPositions().size() + 2,
				            map.GetPositions().size() + 3
				);

				map.AddVertex(Vector3f(i * SPOT_W, SPOT_H, j * SPOT_L));
				map.AddTexCoord(Vector2f(XL, YL));
				map.AddVertex(Vector3f((i + 1) * SPOT_W, SPOT_H, j * SPOT_L));
				map.AddTexCoord(Vector2f(XH, YL));
				map.AddVertex(Vector3f((i + 1) * SPOT_W, SPOT_H, (j + 1) * SPOT_L));
				map.AddTexCoord(Vector2f(XH, YH));
				map.AddVertex(Vector3f(i * SPOT_W, SPOT_H, (j + 1) * SPOT_L));
				map.AddTexCoord(Vector2f(XL, YH));

				texX = ((level.GetPixel(i, j) & 0xFF0000) >> 16) / NUM_TEXTURES;
				texY = texX % NUM_TEX_EXP;
				texX /= NUM_TEX_EXP;

				XH = 1.0f - (float)texX / (float)NUM_TEX_EXP;
				XL = XH - 1 / (float)NUM_TEX_EXP;
				YL = 1.0f - (float)texY / (float)NUM_TEX_EXP;
				YH = YL - 1 / (float)NUM_TEX_EXP;

				if ((level.GetPixel(i, j - 1) & 0xFFFFFF) == 0)
				{
					map.AddFace(map.GetPositions().size() + 0,
					            map.GetPositions().size() + 1,
					            map.GetPositions().size() + 2
					);

					map.AddFace(map.GetPositions().size() + 0,
					            map.GetPositions().size() + 2,
					            map.GetPositions().size() + 3
					);

					map.AddVertex(Vector3f(i * SPOT_W, 0, j * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YL));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, 0, j * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YL));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, SPOT_H, j * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YH));
					map.AddVertex(Vector3f(i * SPOT_W, SPOT_H, j * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YH));
				}
				if ((level.GetPixel(i, j + 1) & 0xFFFFFF) == 0)
				{
					map.AddFace(map.GetPositions().size() + 2,
					            map.GetPositions().size() + 1,
					            map.GetPositions().size() + 0
					);

					map.AddFace(map.GetPositions().size() + 3,
					            map.GetPositions().size() + 2,
					            map.GetPositions().size() + 0
					);

					map.AddVertex(Vector3f(i * SPOT_W, 0, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YL));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, 0, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YL));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, SPOT_H, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YH));
					map.AddVertex(Vector3f(i * SPOT_W, SPOT_H, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YH));
				}
				if ((level.GetPixel(i - 1, j) & 0xFFFFFF) == 0)
				{
					map.AddFace(map.GetPositions().size() + 2,
					            map.GetPositions().size() + 1,
					            map.GetPositions().size() + 0
					);

					map.AddFace(map.GetPositions().size() + 3,
					            map.GetPositions().size() + 2,
					            map.GetPositions().size() + 0
					);

					map.AddVertex(Vector3f(i * SPOT_W, 0, j * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YL));
					map.AddVertex(Vector3f(i * SPOT_W, 0, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YL));
					map.AddVertex(Vector3f(i * SPOT_W, SPOT_H, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YH));
					map.AddVertex(Vector3f(i * SPOT_W, SPOT_H, j * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YH));
				}
				if ((level.GetPixel(i + 1, j) & 0xFFFFFF) == 0)
				{
					map.AddFace(map.GetPositions().size() + 0,
					            map.GetPositions().size() + 1,
					            map.GetPositions().size() + 2
					);

					map.AddFace(map.GetPositions().size() + 0,
					            map.GetPositions().size() + 2,
					            map.GetPositions().size() + 3
					);

					map.AddVertex(Vector3f((i + 1) * SPOT_W, 0, j * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YL));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, 0, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YL));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, SPOT_H, (j + 1) * SPOT_L));
					map.AddTexCoord(Vector2f(XH, YH));
					map.AddVertex(Vector3f((i + 1) * SPOT_W, SPOT_H, j * SPOT_L));
					map.AddTexCoord(Vector2f(XL, YH));
				}
			}
		}
	}
	Mesh mapMesh(mapName + "_map", map.Finalize());

	btTriangleMesh* cols = new btTriangleMesh();

	for (auto x : mapMesh.GetMeshData()->GetModel().GetPositions())
	{
		cols->findOrAddVertex(x.GetBT(), false);
	}

	for (auto x : mapMesh.GetMeshData()->GetModel().GetIndices())
	{
		cols->addIndex(x);
	}

	entity
		->AddComponent(new MeshRenderer(mapMesh, Material(matName)))
		->AddComponent(new RigidBody(new btConvexTriangleMeshShape(cols), 0, false));

	game->AddToScene(entity);
}

Level::~Level()
{
	//dtor
}
