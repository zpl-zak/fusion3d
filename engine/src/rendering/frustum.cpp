#include "frustum.h"

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Plane::setCo(float a, float b, float c, float d)
{
	normal = Vector3f(a, b, c);

	float l = normal.Length();
	normal = Vector3f(a / l, b / l, c / l);

	this->d = d / l;
}

float Plane::distance(Vector3f& p) const
{
	float dist = normal.Dot(p) + d;

	return dist;
}

void Frustum::CalcPlanes(Matrix4f mat)
{
	pl[NEARP].setCo(
		mat[2][0] + mat[3][0],
		mat[2][1] + mat[3][1],
		mat[2][2] + mat[3][2],
		mat[2][3] + mat[3][3]
	);

	pl[FARP].setCo(
		-mat[2][0] + mat[3][0],
		-mat[2][1] + mat[3][1],
		-mat[2][2] + mat[3][2],
		-mat[2][3] + mat[3][3]
	);

	pl[BOTTOM].setCo(
		mat[1][0] + mat[3][0],
		mat[1][1] + mat[3][1],
		mat[1][2] + mat[3][2],
		mat[1][3] + mat[3][3]
	);

	pl[TOP].setCo(
		-mat[1][0] + mat[3][0],
		-mat[1][1] + mat[3][1],
		-mat[1][2] + mat[3][2],
		-mat[1][3] + mat[3][3]
	);

	pl[LEFT].setCo(
		mat[0][0] + mat[3][0],
		mat[0][1] + mat[3][1],
		mat[0][2] + mat[3][2],
		mat[0][3] + mat[3][3]
	);

	pl[RIGHT].setCo(
		-mat[0][0] + mat[3][0],
		-mat[0][1] + mat[3][1],
		-mat[0][2] + mat[3][2],
		-mat[0][3] + mat[3][3]
	);
}

void Frustum::SetCamInternals(float angle, float ratio, float nearD, float farD)
{
	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	tang = tanf(ToRadians(angle * 0.5f));
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD * tang;
	nw = fh * ratio;
}

void Frustum::SetCamDef(Vector3f& p, Vector3f& l, Vector3f& u)
{
}

int Frustum::PointInFrustum(Vector3f& p) const
{
	int r = INSIDE;

	for (int i = 0; i < 6; i++)
		if (pl[i].distance(p) < 0)
			return OUTSIDE;

	return r;
}

int Frustum::SphereInFrustum(Vector3f& p, float radius) const
{
	float distance;
	int r = INSIDE;

	for (int i = 0; i < 6; i++)
	{
		distance = pl[i].distance(p);

		if (distance < -radius)
			return OUTSIDE;
		else if (distance < radius)
			r = INTERSECT;
	}

	return r;
}
