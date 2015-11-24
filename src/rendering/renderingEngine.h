/*
 * Copyright (C) 2014 Dominik Madarasz
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

#ifndef RENDERINGENGINE_H
#define RENDERINGENGINE_H

#include "camera.h"
#include "lighting.h"
#include "material.h"
#include "mesh.h"
#include "window.h"

#include "../core/mappedValues.h"
#include "../core/profiling.h"

#include <vector>
#include <map>
class Entity;

class RenderingEngine : public MappedValues
{
public:
    RenderingEngine(Window* window);
	virtual ~RenderingEngine() 
	{
		delete m_defaultShader;
	}
	
	virtual void Render(const Entity& object);
	
	inline void AddLight(BaseLight* light) { m_lights.push_back(light); }
	inline void SetMainCamera(const Camera& camera) { m_mainCamera = &camera; }
	inline const Camera* GetMainCamera() { return m_mainCamera; }
	
	virtual void UpdateUniformStruct(const Transform& transform, const Material& material, const Shader& shader, 
		const std::string& uniformName, const std::string& uniformType) const
	{
		throw uniformType + " is not supported by the rendering engine";
	}
	
	inline double DisplayRenderTime(double dividend) { return m_renderProfileTimer.DisplayAndReset("Render Time: ", dividend); }
	inline double DisplayWindowSyncTime(double dividend) { return m_windowSyncProfileTimer.DisplayAndReset("Window Sync Time: ", dividend); }
	inline void SetShader(const std::string& fileName) { delete m_defaultShader; m_defaultShader = new Shader(fileName); }
	inline void SetFullBright(bool state) { m_fullbright = state; }
	
	inline const BaseLight& GetActiveLight()                           const { return *m_activeLight; }
	inline unsigned int GetSamplerSlot(const std::string& samplerName) const { return m_samplerMap.find(samplerName)->second; }
	inline const Matrix4f& GetLightMatrix()                            const { return m_lightMatrix; }
protected:
	inline void SetSamplerSlot(const std::string& name, unsigned int value) { m_samplerMap[name] = value; }
private:
	static const int NUM_SHADOW_MAPS = 10;
	static const Matrix4f BIAS_MATRIX;
	
	bool								m_fullbright;
	ProfileTimer                        m_renderProfileTimer;
	ProfileTimer                        m_windowSyncProfileTimer;
	Transform                           m_planeTransform;
	Mesh                                m_plane;
	
	Window*                       m_window;
	SDL_Renderer*						m_gui;
	SDL_Surface*						m_surface;
	Texture                             m_tempTarget;
	Material                            m_planeMaterial;
	Texture                             m_shadowMaps[NUM_SHADOW_MAPS];
	Texture                             m_shadowMapTempTargets[NUM_SHADOW_MAPS];
	
	Shader*                              m_defaultShader;
	Shader								m_nullShader;
	Shader                              m_shadowMapShader;
	Shader                              m_nullFilter;
	Shader                              m_gausBlurFilter;
	Shader                              m_fxaaFilter;
	Matrix4f                            m_lightMatrix;
	
	Transform                           m_altCameraTransform;
	Camera                              m_altCamera;
	const Camera*                       m_mainCamera;
	BaseLight*                    m_activeLight;
	std::vector<BaseLight*>       m_lights;
	std::map<std::string, unsigned int> m_samplerMap;
	
	void BlurShadowMap(int shadowMapIndex, float blurAmount);
	void ApplyFilter(const Shader& filter, const Texture& source, const Texture* dest);
	void drawrect(int x, int y, int w, int h, int color);

	RenderingEngine(const RenderingEngine& other) :
		m_altCamera(Matrix4f(),0){}
	void operator=(const RenderingEngine& other) {}
};

class GLDebugDrawer : public btIDebugDraw
{
	int m_debugMode = 1;

public:

	GLDebugDrawer();

	virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void   reportErrorWarning(const char* warningString);

	virtual void   draw3dText(const btVector3& location, const char* textString);

	virtual void   setDebugMode(int debugMode);

	virtual int      getDebugMode() const { return m_debugMode; }

};

#endif // RENDERINGENGINE_H
