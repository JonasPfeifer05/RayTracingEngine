#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "..\performance/aabb.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittableList : public hittable {
public:
    hittableList() {}
    hittableList(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }


    virtual bool hit(
        const ray& r, double tMin, double tMax, hitRecord& rec) const override;
    virtual bool boundingBox(
        double time0, double time1, aabb& outputBox) const override;

public:
    std::vector<shared_ptr<hittable>> objects;
};

bool hittableList::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    hitRecord tempRec;
    bool hitAnything = false;
    auto closestSoFar = tMax;

    for (const auto& object : objects) {
        if (object->hit(r, tMin, closestSoFar, tempRec)) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}

bool hittableList::boundingBox(double time0, double time1, aabb& outputBox) const {
    if (objects.empty()) return false;

    aabb tempBox;
    bool firstBox = true;

    for (const auto& object : objects) {
        if (!object->boundingBox(time0, time1, tempBox)) return false;
        outputBox = firstBox ? tempBox : surroundingBox(outputBox, tempBox);
        firstBox = false;
    }

    return true;
}

#endif