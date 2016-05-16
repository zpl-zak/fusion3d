#if !defined(FRUSTUM_H_INCLUDED)

#include "../core/math3d.h"

class Plane
{
public:

	float d;
	Vector3f normal;

	void setCo(float, float, float, float);
	float distance(Vector3f &p) const;
};

class Frustum
{
public:
	static enum {OUTSIDE, INTERSECT, INSIDE};
	Plane pl[6];
	Vector3f ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;

	Frustum();
	~Frustum();
	void CalcPlanes(Matrix4f mat);
	void SetCamInternals(float angle, float ratio, float nearD, float farD);
	void SetCamDef(Vector3f &p, Vector3f &l, Vector3f &u);
	int PointInFrustum(Vector3f &p) const;
	int SphereInFrustum(Vector3f &p, float radius) const;
	//int PointInFrustum(Vector3f &p); //TODO(zaklaus): Implement AABox class!

private:
	enum {TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP};
};

#define FRUSTUM_H_INCLUDED 1
#endif
