#ifndef WORLDCOMPONENT_H_INCLUDED
#define WORLDCOMPONENT_H_INCLUDED "WorldComponent"

#include <vector>

#include "meshRenderer.h"
#include "../rendering/lighting.h"
#include "programComponent.h"

COMPONENT(WorldComponent)
public:
	WorldComponent()
	{

	}

	virtual void Init() 
	{
		m_model->Init();

		for (auto progs : m_progs)
		{
			progs->Init();
		}
	}
	virtual void ProcessInput(const Input& input, float delta)
	{
		for (auto progs : m_progs)
		{
			progs->ProcessInput(input, delta);
		}
	}
	virtual void Update(float delta)
	{
		m_model->Update(delta);

		for (auto progs : m_progs)
		{
			progs->Update(delta);
		}
	}
	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const 
	{
		m_model->Render(shader, renderingEngine, camera);

		for (auto progs : m_progs)
		{
			progs->Render(shader, renderingEngine, camera);
		}
	}

	WorldComponent* SetUpModel(MeshRenderer* model) 
	{
		m_model = model;

		return this;
	}

	WorldComponent* SetUpLights(Entity* lights)
	{
		m_lights = lights;
		
		return this;
	}

	WorldComponent* AddProgram(ProgramComponent* prog)
	{
		m_progs.push_back(prog);

		return this;
	}

private:
	MeshRenderer* m_model;
	Entity* m_lights;
	std::vector<ProgramComponent*> m_progs;
};

#endif
