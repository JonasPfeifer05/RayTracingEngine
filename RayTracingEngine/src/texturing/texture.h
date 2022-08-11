#ifndef TEXTURE_H
#define TEXTURE_H

#include "../utils/rtweekend.h"
#include "perlin.h"
#include "../utils/rtw_stb_image.h"
#include "perlin.h"

#include <iostream>

class texture {
public:
    virtual color value(double u, double v, const point3& p) const = 0;
};

class solidColor : public texture {
public:
    solidColor() {}
    solidColor(color c) : colorValue(c) {}

    solidColor(double red, double green, double blue)
        : solidColor(color(red, green, blue)) {}

    virtual color value(double u, double v, const vec3& p) const override {
        return colorValue;
    }

private:
    color colorValue;
};

class checkerTexture : public texture {
public:
    checkerTexture() {}

    checkerTexture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : even(_even), odd(_odd) {}

    checkerTexture(color c1, color c2)
        : even(make_shared<solidColor>(c1)), odd(make_shared<solidColor>(c2)) {}

    virtual color value(double u, double v, const point3& p) const override {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

class noiseTexture : public texture {
public:
    noiseTexture() {}
    noiseTexture(double sc) : scale(sc) {}

    virtual color value(double u, double v, const point3& p) const override {
        return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }

public:
    perlin noise;
    double scale;
};

class imageTexture : public texture {
public:
    const static int bytesPerPixel = 3;

    imageTexture()
        : data(nullptr), width(0), height(0), bytesPerScanline(0) {}

    imageTexture(const char* filename) {
        auto componentsPerPixel = bytesPerPixel;

        data = stbi_load(
            filename, &width, &height, &componentsPerPixel, componentsPerPixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytesPerScanline = bytesPerPixel * width;
    }

    ~imageTexture() {
        delete data;
    }

    virtual color value(double u, double v, const vec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto colorScale = 1.0 / 255.0;
        auto pixel = data + j * bytesPerScanline + i * bytesPerPixel;

        return color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytesPerScanline;
};

#endif