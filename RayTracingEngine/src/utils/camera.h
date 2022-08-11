#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
public:
    camera() {};
    camera(
        point3 lookfrom,
        point3 lookat,
        vec3   vup,
        double vfov, // vertical field-of-view in degrees
        double aspectRatio,
        double aperture,
        double focusDist,
        double _time0 = 0,
        double _time1 = 0
    ) {
        auto theta = degreesToRadians(vfov);
        auto h = tan(theta / 2);
        auto viewportHeight = 2.0 * h;
        auto viewportWidth = aspectRatio * viewportHeight;

        w = unitVector(lookfrom - lookat);
        u = unitVector(cross(vup, w));
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focusDist * viewportWidth * u;
        vertical = focusDist * viewportHeight * v;
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - focusDist * w;

        lensRadius = aperture / 2;

        time0 = _time0;
        time1 = _time1;
    }

    ray get_ray(double s, double t) const {
        vec3 rd = lensRadius * randomInUnitDisk();
        vec3 offset = u * rd.x() + v * rd.y();

        return ray(
            origin + offset,
            lowerLeftCorner + s * horizontal + t * vertical - origin - offset,
            randomDouble(time0, time1)
        );
    }

private:
    point3 origin;
    point3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double time0, time1;  // shutter open/close times
    double lensRadius;
};
#endif