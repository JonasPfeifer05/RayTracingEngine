#ifndef MATERIAL_H
#define MATERIAL_H

#include "../utils/rtweekend.h"
#include "../objects/hittable.h"
#include "texture.h"

class material {
public:
    virtual bool scatter(
        const ray& rIn, const hitRecord& rec, color& attenuation, ray& scattered
    ) const = 0;
    virtual color emitted(double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }

};

class lambertian : public material {
public:
    lambertian(const color& a) : albedo(make_shared<solidColor>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(
        const ray& rIn, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        auto scatterDirection = rec.normal + randomUnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.near_zero())
            scatterDirection = rec.normal;

        scattered = ray(rec.p, scatterDirection, rIn.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

class metal : public material {
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& rIn, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        vec3 reflected = reflect(unitVector(rIn.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * randomInUnitSphere(), rIn.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double indexOfRefraction) : ir(indexOfRefraction) {}

    virtual bool scatter(
        const ray& r_in, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refractionRatio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unitDirection = unitVector(r_in.direction());
        double cosTheta = fmin(dot(-unitDirection, rec.normal), 1.0);
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        vec3 direction;

        if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble())
            direction = reflect(unitDirection, rec.normal);
        else
            direction = refract(unitDirection, rec.normal, refractionRatio);

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

public:
    double ir; // Index of Refraction

private:
    static double reflectance(double cosine, double refIdx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refIdx) / (1 + refIdx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class diffuseLight : public material {
public:
    diffuseLight(shared_ptr<texture> a) : emit(a) {}
    diffuseLight(color c) : emit(make_shared<solidColor>(c)) {}

    virtual bool scatter(
        const ray& rIn, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};

class isotropic : public material {
public:
    isotropic(color c) : albedo(make_shared<solidColor>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(
        const ray& rIn, const hitRecord& rec, color& attenuation, ray& scattered
    ) const override {
        scattered = ray(rec.p, randomInUnitSphere(), rIn.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};
#endif