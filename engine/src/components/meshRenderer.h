// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef MESHRENDERER_H_INCLUDED
#define MESHRENDERER_H_INCLUDED

#include "../core/entityComponent.h"
#include "../core/timing.h"
#include "../rendering/mesh.h"
#include "../rendering/renderingEngine.h"

#include <Windows.h>


class MeshRenderer : public EntityComponent
{
public:
	FCLASS (MeshRenderer);
	MeshRenderer();
	MeshRenderer(std::vector<Mesh*> meshes, std::vector<Material*> materials);

	MeshRenderer(const std::string& fileName);

	MeshRenderer(Mesh mesh, const Material& material);

	virtual void Init();

	virtual void Render(Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);

	virtual void DataDeploy(tinyxml2::XMLElement* data);

	virtual void DrawDebugUI();

	EntityComponent* SetVisible(bool visible);

	bool IsVisible();

	void LoadData(std::vector<Mesh*> meshes, std::vector<Material*> materials);

	EntityComponent* SetShadows(bool shadows);

	bool HasShadows();

	Material& GetMaterial(int ID);
	Mesh& GetMesh(int ID);

protected:
	std::vector<Mesh> m_mesh;
	std::vector<Material> m_material;
	std::vector<const char*> m_matNames;
	bool			m_visible;
	bool			m_shadows;
private:
};

class MeshSequence : public MeshRenderer
{
public:
	FCLASS (MeshSequence);
	MeshSequence();

	MeshSequence(const std::string& fileName, int stepSize);

	void Update(float delta);

	void Render(const Transform& transform, Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);

	void DataDeploy(tinyxml2::XMLElement* data);

	EntityComponent* SetVisible(bool visible);

	bool IsVisible();


	void LoadData(std::string filename, size_t stepSize);


	EntityComponent* SetShadows(bool shadows);

	bool HasShadows();

	std::string GetName();

	void SetStep(size_t step);

	void SetPlay(bool play);

protected:
private:
	std::vector<std::vector<Mesh>> m_meshes;
	std::vector<Material> m_materials;
	std::string m_name;
	bool m_play;
	size_t m_step;
	size_t m_skip;
	DelayTimer m_timer;
};

class AnimMeshRenderer : public EntityComponent
{
public:
	AnimMeshRenderer();

	AnimMeshRenderer(const std::string& fileName, int stepSize);

	virtual ~AnimMeshRenderer();

	virtual void Update(float delta);

	void Render(Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const;

	virtual void DataDeploy(tinyxml2::XMLElement* data);

	void LoadData(std::string filename, int stepSize);

	void SetAnimState(std::string anim)
	{
		int i = 0;
		for (auto x : m_anims)
		{
			if (x->GetName() == anim)
			{
				m_anim = i;
				m_anims.at(i)->SetStep(-1);
				return;
			}
			++i;
		}
	}

	std::string GetAnimState() 
	{
		return m_anims.at(m_anim)->GetName();
	}

	void SetPlayState(bool play)
	{
		m_anims.at(m_anim)->SetPlay(play);
	}

protected:
private:
	std::vector<MeshSequence*> m_anims;
	int m_anim;
};

#endif // MESHRENDERER_H_INCLUDED
