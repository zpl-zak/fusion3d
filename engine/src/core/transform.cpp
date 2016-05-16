// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#include "transform.h"
#include "coreEngine.h"

bool Transform::HasChanged() const
{	
	if (m_isInitial)
	{
		return true;
	}

	if(m_parent != 0 && m_parent->HasChanged())
	{
		//CoreEngine::GetCoreEngine ()->GetRenderingEngine ()->UpdateUniforms (true);
		return true;
	}
	
	if(m_pos != m_oldPos)
	{
		//CoreEngine::GetCoreEngine ()->GetRenderingEngine ()->UpdateUniforms (true);
		return true;
	}

	if(m_rot != m_oldRot)
	{
		//CoreEngine::GetCoreEngine ()->GetRenderingEngine ()->UpdateUniforms (true);
		return true;
	}
	
	if(m_scale != m_oldScale)
	{
		//CoreEngine::GetCoreEngine ()->GetRenderingEngine ()->UpdateUniforms (true);
		return true;
	}
		
	return false;
}

void Transform::Update()
{
	if(m_initializedOldStuff)
	{
		m_oldPos = m_pos;
		m_oldRot = m_rot;
		m_oldScale = m_scale;
	}
	else
	{
		m_oldPos = m_pos + Vector3f(1,1,1);
		m_oldRot = m_rot * 0.5f;
		m_oldScale = m_scale + 1;
		m_initializedOldStuff = true;
	}

	m_isInitial = false;
}

void Transform::Rotate(const Vector3f& axis, float angle)
{
	Rotate(Quaternion(axis, angle));
	m_hasChanged = true;
}

void Transform::Rotate(const Quaternion& rotation)
{
	m_rot = Quaternion((rotation * m_rot).Normalized());
	m_hasChanged = true;
}

void Transform::LookAt(const Vector3f& point, const Vector3f& up)
{
	m_rot = GetLookAtRotation(point, up);
	m_hasChanged = true;
}

Matrix4f Transform::GetTransformation() const
{
	Matrix4f translationMatrix;
	Matrix4f scaleMatrix;

	translationMatrix.InitTranslation(Vector3f(m_pos.GetX(), m_pos.GetY(), m_pos.GetZ()));
	scaleMatrix.InitScale(Vector3f(m_scale, m_scale, m_scale));

	Matrix4f result = translationMatrix * m_rot.ToRotationMatrix() * scaleMatrix;

	return GetParentMatrix() * result;
}

const Matrix4f& Transform::GetParentMatrix() const
{
	if(m_parent != 0 && m_parent->HasChanged())
	{
		m_parentMatrix = m_parent->GetTransformation();
	}
		
	return m_parentMatrix;
}

Quaternion Transform::GetTransformedRot() const
{
	Quaternion parentRot = Quaternion(0,0,0,1);
	
	if(m_parent)
	{
		parentRot = m_parent->GetTransformedRot();
	}
	
	return parentRot * m_rot;
}
