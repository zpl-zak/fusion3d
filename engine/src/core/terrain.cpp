#include "terrain.h"

#include "../3DEngine.h"
#include "../rendering/mesh.h"


Terrain::Terrain (std::string mapName, int height, bool flat)
{
	Bitmap level (mapName); level.FlipY ();

	constexpr int SPOT_W = 1;
	constexpr int SPOT_L = 1;
	const int SPOT_H = height;

	int MaxY = 0;

	for (int i = 0; i < level.GetWidth (); i++)
	{
		for (int j = 0; j < level.GetHeight (); j++)
		{
			const int Y = ((level.GetPixel (i, j) & 0xFF0000) >> 16);

			if (MaxY < Y) MaxY = Y;
		}
	}

	IndexedModel map;
	{
		for (int i = 0; i < level.GetWidth (); i++)
		{
			for (int j = 0; j < level.GetHeight(); j++)
			{
				const float Y1 = ((float)((level.GetPixel (i, j) & 0xFF0000) >> 16) / (float)MaxY) * SPOT_H;
				const float Y2 = ((float)((level.GetPixel (i+1, j) & 0xFF0000) >> 16) / (float)MaxY) * SPOT_H;
				const float Y3 = ((float)((level.GetPixel (i+1, j+1) & 0xFF0000) >> 16) / (float)MaxY) * SPOT_H;
				const float Y4 = ((float)((level.GetPixel (i, j+1) & 0xFF0000) >> 16) / (float)MaxY) * SPOT_H;

				int XH = 1;// (float)(i) / (float)level.GetWidth ();
				int XL = 0;// (float)i / (float)level.GetWidth ();
				int YH = 1;// (float)(j) / (float)level.GetHeight ();
				int YL = 0;// (float)j / (float)level.GetHeight ();

				map.AddFace (
					map.GetPositions ().size () + 2,
					map.GetPositions ().size () + 1,
					map.GetPositions ().size () + 0
					);

				map.AddFace (
					map.GetPositions ().size () + 3,
					map.GetPositions ().size () + 2,
					map.GetPositions ().size () + 0
					);

				map.AddVertex (Vector3f (i * SPOT_W, Y1, j * SPOT_L)); map.AddTexCoord (Vector2f (XL, YL));
				map.AddVertex (Vector3f ((i + 1) * SPOT_W, Y2, j * SPOT_L)); map.AddTexCoord (Vector2f (XH, YL));
				map.AddVertex (Vector3f ((i + 1) * SPOT_W, Y3, (j + 1) * SPOT_L)); map.AddTexCoord (Vector2f (XH, YH));
				map.AddVertex (Vector3f (i * SPOT_W, Y4, (j + 1) * SPOT_L)); map.AddTexCoord (Vector2f (XL, YH));
				
				/*auto slots = std::vector<unsigned int>{ (unsigned int) j*level.GetWidth() + i ,(unsigned int)j*level.GetWidth() + i ,(unsigned int)j*level.GetWidth() + i ,(unsigned int)j*level.GetWidth() + i };

				map.AddVertexSlot(slots);
				map.AddVertexSlot(slots);
				map.AddVertexSlot(slots);
				map.AddVertexSlot(slots);*/
			}
		}

		m_mesh = new Mesh (mapName, map.Finalize(flat));
	}
}

Terrain::~Terrain ()
{
}

void Terrain::Start(Entity* entity, std::string matName)
{
	entity
		->AddComponent(new MeshRenderer(*m_mesh, Material(matName)));

	//game->AddToScene(entity);
}
