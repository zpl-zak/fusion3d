#pragma once


#include "3DEngine.h"

class PLACEHOLDERRenderer : public RenderingEngine
{
public:
	PLACEHOLDERRenderer (Window* window);
	~PLACEHOLDERRenderer ();

	void Render (Entity& object) override;
};