// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef CAMERA_H
#define CAMERA_H "Camera"

#include "../core/math3d.h"
#include "../core/entityComponent.h"
#include "frustum.h"


/**
 * Cameras represent a location, orientation, and projection from
 * which the scene can be rendered.
 */
class Camera
{
public:
	Camera() {}
	//Transform is passed in so the Camera doesn't need to be attached to a game object.
	//That's useful for places such as the rendering engine which can use cameras
	//without creating placeholder game objects.
	Camera(const Matrix4f& projection, Transform* transform) :
		m_projection(projection),
		m_transform(transform) {}
	
	inline Transform* GetTransform()             { return m_transform; }
	inline const Transform& GetTransform() const { return *m_transform; }
	inline const Frustum& GetFrustum() const { return m_frustum; }
	//This is the primary function of the camera. Multiplying a point by the returned matrix
	//will transform the point into it's location on the screen, where -1 represents the bottom/left
	//of the screen, and 1 represents the top/right of the screen.
	Matrix4f GetViewProjection()           const;

	inline void SetProjection(const Matrix4f& projection) { m_projection = projection; }
	inline Matrix4f GetCachedViewProjection() const { return m_cachedVP; }
	inline void SetTransform(Transform* transform) { m_transform = transform; /*m_frustum.CalcPlanes(GetViewProjection());*/  }
protected:
private:
	Matrix4f   m_projection; //The projection with which the camera sees the world (i.e. perspective, orthographic, identity, etc.)
	Transform* m_transform;  //The transform representing the position and orientation of the camera.
	Frustum    m_frustum;
	Matrix4f m_cachedVP;
};

//CameraComponents are an easy way to use a camera as a component
//on a game object.
class CameraComponent : public EntityComponent
{
public:
	FCLASS (CameraComponent);

	CameraComponent() {}
	CameraComponent(const Matrix4f& projection) :
		m_camera(projection, 0) {}
	
	virtual void AddToEngine(CoreEngine* engine);
	
	inline Matrix4f GetViewProjection() const { return m_camera.GetViewProjection(); }
	
	inline void SetProjection(const Matrix4f& projection) { m_camera.SetProjection(projection); }
	virtual void SetParent(Entity* parent);
protected:
private:
	Camera m_camera; //The camera that's being used like a component.
};


#endif
