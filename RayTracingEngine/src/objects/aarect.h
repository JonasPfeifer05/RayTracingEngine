#ifndef AARECT_H
#define AARECT_H

#include "..\utils/rtweekend.h"

#include "hittable.h"

class xyRect : public hittable {
public:
    xyRect() {}

    xyRect(double _x0, double _x1, double _y0, double _y1, double _k,
        shared_ptr<material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

    virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

    virtual bool boundingBox(double time0, double time1, aabb& outputBox) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        outputBox = aabb(point3(x0, y0, k - 0.0001), point3(x1, y1, k + 0.0001));
        return true;
    }

public:
    shared_ptr<material> mp;
    double x0, x1, y0, y1, k;
};

class xzRect : public hittable {
public:
    xzRect() {}

    xzRect(double _x0, double _x1, double _z0, double _z1, double _k,
        shared_ptr<material> mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

    virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

    virtual bool boundingBox(double time0, double time1, aabb& outputBox) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Y
        // dimension a small amount.
        outputBox = aabb(point3(x0, k - 0.0001, z0), point3(x1, k + 0.0001, z1));
        return true;
    }

public:
    shared_ptr<material> mp;
    double x0, x1, z0, z1, k;
};

class yzRect : public hittable {
public:
    yzRect() {}

    yzRect(double _y0, double _y1, double _z0, double _z1, double _k,
        shared_ptr<material> mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

    virtual bool hit(const ray& r, double tMin, double tMax, hitRecord& rec) const override;

    virtual bool boundingBox(double time0, double time1, aabb& outputBox) const override {
        // The bounding box must have non-zero width in each dimension, so pad the X
        // dimension a small amount.
        outputBox = aabb(point3(k - 0.0001, y0, z0), point3(k + 0.0001, y1, z1));
        return true;
    }

public:
    shared_ptr<material> mp;
    double y0, y1, z0, z1, k;
};

bool xyRect::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    auto t = (k - r.origin().z()) / r.direction().z();
    if (t < tMin || t > tMax)
        return false;
    auto x = r.origin().x() + t * r.direction().x();
    auto y = r.origin().y() + t * r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    auto outwardNormal = vec3(0, 0, 1);
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = mp;
    rec.p = r.at(t);
    return true;
}

bool xzRect::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    auto t = (k - r.origin().y()) / r.direction().y();
    if (t < tMin || t > tMax)
        return false;
    auto x = r.origin().x() + t * r.direction().x();
    auto z = r.origin().z() + t * r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outwardNormal = vec3(0, 1, 0);
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = mp;
    rec.p = r.at(t);
    return true;
}

bool yzRect::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    auto t = (k - r.origin().x()) / r.direction().x();
    if (t < tMin || t > tMax)
        return false;
    auto y = r.origin().y() + t * r.direction().y();
    auto z = r.origin().z() + t * r.direction().z();
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outwardNormal = vec3(1, 0, 0);
    rec.setFaceNormal(r, outwardNormal);
    rec.matPtr = mp;
    rec.p = r.at(t);
    return true;
}

#endif