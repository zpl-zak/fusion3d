// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math3d.h"

/**
 * Holds information about current world-offset.
 */
class Transform
{
public:
	Transform(const Vector3f& pos = Vector3f(0,0,0), const Quaternion& rot = Quaternion(0,0,0,1), float scale = 1.0f) :
		m_pos(pos),
		m_rot(rot),
		m_scale(scale),
		m_parent(0),
		m_parentMatrix(Matrix4f().InitIdentity()),
		m_initializedOldStuff(false),
		m_isInitial(true) {}

	Matrix4f GetTransformation() const;
	bool HasChanged() const;
	void Update();
	void Rotate(const Vector3f& axis, float angle);
	void Rotate(const Quaternion& rotation);
	void LookAt(const Vector3f& point, const Vector3f& up);
	
	Quaternion GetLookAtRotation(const Vector3f& point, const Vector3f& up) 
	{ 
		return Quaternion(Matrix4f().InitRotationFromDirection((point - m_pos).Normalized(), up)); 
	}
	
	inline Vector3f* GetPos()                   { return &m_pos; }
	inline const Vector3f& GetPos()       const { return m_pos; }
	inline Quaternion* GetRot()                 { return &m_rot; }
	inline const Quaternion& GetRot()     const { return m_rot; }
	inline float GetScale()               const { return m_scale; }
	inline Vector3f GetTransformedPos()   const { return Vector3f(GetParentMatrix().Transform(m_pos)); }
	Quaternion GetTransformedRot()        const;

	inline btTransform GetBT()
	{
		return btTransform(GetRot()->GetBT(), GetPos()->GetBT());
	}

	inline static Transform GetFT(const btTransform& bt)
	{
		return Transform(Vector3f::GetFT(bt.getOrigin()), Quaternion::GetFT(bt.getRotation()));
	}

	inline void SetPos(const Vector3f& pos) { m_pos = pos; m_hasChanged = true;}
	inline void SetRot(const Quaternion& rot) { m_rot = rot; m_hasChanged = true;}
	inline void SetScale(float scale) { m_scale = scale; m_hasChanged = true;}
	inline void SetParent(Transform* parent) { m_parent = parent; m_hasChanged = true;}

	bool m_hasChanged;
protected:
private:
	const Matrix4f& GetParentMatrix() const;

	Vector3f m_pos;
	Quaternion m_rot;
	float m_scale;
	bool m_isInitial;
	
	
	Transform* m_parent;
	mutable Matrix4f m_parentMatrix;
	
	mutable Vector3f m_oldPos;
	mutable Quaternion m_oldRot;
	mutable float m_oldScale;
	mutable bool m_initializedOldStuff;
};

#endif
