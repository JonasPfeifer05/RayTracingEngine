#ifndef BVH_H
#define BVH_H

#include "../utils/rtweekend.h"

#include "../objects/hittable.h"
#include "../objects/hittableList.h"


class bvhNode : public hittable {
public:
    bvhNode();

    bvhNode(const hittableList& list, double time0, double time1)
        : bvhNode(list.objects, 0, list.objects.size(), time0, time1)
    {}

    bvhNode(
        const std::vector<shared_ptr<hittable>>& src_objects,
        size_t start, size_t end, double time0, double time1);

    virtual bool hit(
        const ray& r, double tMin, double tMax, hitRecord& rec) const override;

    virtual bool boundingBox(double time0, double time1, aabb& outputBox) const override;

public:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb box;
};

bool bvhNode::boundingBox(double time0, double time1, aabb& outputBox) const {
    outputBox = box;
    return true;
}

bool bvhNode::hit(const ray& r, double tMin, double tMax, hitRecord& rec) const {
    if (!box.hit(r, tMin, tMax))
        return false;

    bool hitLeft = left->hit(r, tMin, tMax, rec);
    bool hitRight = right->hit(r, tMin, hitLeft ? rec.t : tMax, rec);

    return hitLeft || hitRight;
}

inline bool boxCompare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    aabb boxA;
    aabb boxB;

    if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return boxA.min().e[axis] < boxB.min().e[axis];
}


bool boxXCompare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return boxCompare(a, b, 0);
}

bool boxYCompare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return boxCompare(a, b, 1);
}

bool boxZCompare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return boxCompare(a, b, 2);
}

bvhNode::bvhNode(
    const std::vector<shared_ptr<hittable>>& srcObjects,
    size_t start, size_t end, double time0, double time1
) {
    auto objects = srcObjects; // Create a modifiable array of the source scene objects

    int axis = randomInt(0, 2);
    auto comparator = (axis == 0) ? boxXCompare
        : (axis == 1) ? boxYCompare
        : boxZCompare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    }
    else if (object_span == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            left = objects[start];
            right = objects[start + 1];
        }
        else {
            left = objects[start + 1];
            right = objects[start];
        }
    }
    else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        left = make_shared<bvhNode>(objects, start, mid, time0, time1);
        right = make_shared<bvhNode>(objects, mid, end, time0, time1);
    }

    aabb boxLeft, boxRight;

    if (!left->boundingBox(time0, time1, boxLeft)
        || !right->boundingBox(time0, time1, boxRight)
        )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surroundingBox(boxLeft, boxRight);
}

#endif