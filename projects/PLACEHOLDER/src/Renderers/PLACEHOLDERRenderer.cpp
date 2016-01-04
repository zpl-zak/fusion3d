#include "PLACEHOLDERRenderer.h"

PLACEHOLDERRenderer::PLACEHOLDERRenderer(Window* window) :
	RenderingEngine (window)
{
	// Set ambient color
	SetVector3f ("ambient", Vector3f (0.1, 0.1, 0.12));
}

PLACEHOLDERRenderer::~PLACEHOLDERRenderer ()
{

}

void PLACEHOLDERRenderer::Render (const Entity & object)
{
	RenderingEngine::Render (object);
}
