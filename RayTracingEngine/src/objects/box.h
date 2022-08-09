#ifndef BOX_H
#define BOX_H

#include "..\utils/rtweekend.h"

#include "aarect.h"
#include "hittableList.h"

class box : public hittable {
public:
    box() {}
    box(const point3& p0, const point3& p1, shared_ptr<material> ptr);

    virtual bool hit(const ray& r, double tMin, double tmax, hitRecord& rec) const override;

    virtual bool boundingBox(double time0, double time1, aabb& outputBox) const override {
        outputBox = aabb(boxMin, boxMax);
        return true;
    }

public:
    point3 boxMin;
    point3 boxMax;
    hittableList sides;
};

box::box(const point3& p0, const point3& p1, shared_ptr<material> ptr) {
    boxMin = p0;
    boxMax = p1;

    sides.add(make_shared<xyRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(make_shared<xyRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    sides.add(make_shared<xzRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(make_shared<xzRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    sides.add(make_shared<yzRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(make_shared<yzRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool box::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    return sides.hit(r, tMin, tMax, rec);
}

#endif