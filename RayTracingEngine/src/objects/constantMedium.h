#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "..\utils/rtweekend.h"

#include "hittable.h"
#include "..\texturing/material.h"
#include "..\texturing/texture.h"

class constantMedium : public hittable {
public:
    constantMedium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
        : boundary(b),
        negInvDensity(-1 / d),
        phaseFunction(make_shared<isotropic>(a))
    {}

    constantMedium(shared_ptr<hittable> b, double d, color c)
        : boundary(b),
        negInvDensity(-1 / d),
        phaseFunction(make_shared<isotropic>(c))
    {}

    virtual bool hit(
        const ray& r, double tMin, double tMax, hitRecord& rec) const override;

    virtual bool boundingBox(double time0, double time1, aabb& outputBox) const override {
        return boundary->boundingBox(time0, time1, outputBox);
    }

public:
    shared_ptr<hittable> boundary;
    shared_ptr<material> phaseFunction;
    double negInvDensity;
};

bool constantMedium::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    
    hitRecord rec1, rec2;

    if (!boundary->hit(r, -infinity, infinity, rec1))
        return false;

    if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))
        return false;

    if (rec1.t < tMin) rec1.t = tMin;
    if (rec2.t > tMax) rec2.t = tMax;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    const auto rayLength = r.direction().length();
    const auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
    const auto hitDistance = negInvDensity * log(randomDouble());

    if (hitDistance > distanceInsideBoundary)
        return false;

    rec.t = rec1.t + hitDistance / rayLength;
    rec.p = r.at(rec.t);

    rec.normal = vec3(1, 0, 0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.matPtr = phaseFunction;

    return true;
}

#endif