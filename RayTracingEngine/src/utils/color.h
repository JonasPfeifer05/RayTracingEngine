#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

void writeColor(std::ostream& out, color pixelColor, int samplesPerPixel) {
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samplesPerPixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);


    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

void translateColor(color& color, int samplesPerPixel) {
    auto scale = 1.0 / samplesPerPixel;

    color.e[0] = sqrt(scale * color.e[0]);
    color.e[1] = sqrt(scale * color.e[1]);
    color.e[2] = sqrt(scale * color.e[2]);

    color.e[0] = static_cast<int>(256 * clamp(color.e[0], 0.0, 0.999));
    color.e[1] = static_cast<int>(256 * clamp(color.e[1], 0.0, 0.999));
    color.e[2] = static_cast<int>(256 * clamp(color.e[2], 0.0, 0.999));
}

void writeColorStream(std::ostream& out, color color) {
    out << color.e[0] << " " << color.e[1] << " " << color.e[2] << "\n";
}
#endif