
#include "Sphere.h"
#include<stdio.h>
bool Sphere::hit(const Ray& r, HitRec& rec) const {

	//Now we need to find out the t
	Vec3f oc = r.o - c;
	float A = r.d.dot(r.d);
	float B = 2.0f*oc.dot(r.d);
	float C = oc.dot(oc) - (this->r*this->r);

	float det = B * B - 4.0f * A * C;

	if (det < 0) {
		return false;
	}

	float sqrtDet = sqrtf(det);
	float t1 = (-B + sqrtDet) / 2.0f;

	float t2 = (-B - sqrtDet) / 2.0f;

	float valid_t = r.tClip;

	if (t1 < 0 && t2 < 0) {
		return false;
	}
	if (t1 > 0 && t2 > 0) {
		valid_t = fminf(t1, t2);
	}

	if (t1 < 0 || t2 < 0) {
		valid_t = fmaxf(t1, t2);
	}
	if (valid_t > r.tClip || (rec.anyHit && valid_t>rec.tHit)) {
		return false;
	}
	rec.tHit = valid_t;


	rec.anyHit = true;

	return true;
}


void Sphere::computeSurfaceHitFields(const Ray & r, HitRec & rec) const {
	rec.p = r.o + r.d * rec.tHit;
	rec.n = (rec.p - c).normalize();
}

const Material* Sphere::getMaterial() const {
	return material;
}
