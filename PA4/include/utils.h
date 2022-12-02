#ifndef UTILS_H_
#define UTILS_H_

#include "core.h"

#include <random>

namespace utils {

    static inline float clamp01(float v) {
        if (v > 1) v = 1;
        else if (v < 0) v = 0;
        return v;
    }

    static inline uint8_t gammaCorrection(float radiance) {
        return static_cast<uint8_t>(255.f * clamp01(powf(radiance, 1.f / 2.2f)));
    }

    static inline float radians(float x) { return x * PI / 180; }

    static inline Vec3f deNan(const Vec3f &vec, float val) {
        Vec3f tmp = vec;
        if (vec.x() != vec.x()) tmp.x() = val;
        if (vec.y() != vec.y()) tmp.y() = val;
        if (vec.z() != vec.z()) tmp.z() = val;
        return tmp;
    }

    static inline unsigned int expandBits(unsigned int v) {
        v = (v * 0x00010001u) & 0xFF0000FFu;
        v = (v * 0x00000101u) & 0x0F00F00Fu;
        v = (v * 0x00000011u) & 0xC30C30C3u;
        v = (v * 0x00000005u) & 0x49249249u;
        return v;
    }

    static inline unsigned int morton3D(Vec3f v) {
        float x = v.x();
        float y = v.y();
        float z = v.z();
        x = std::min(std::max(x * 1024.0f, 0.0f), 1023.0f);
        y = std::min(std::max(y * 1024.0f, 0.0f), 1023.0f);
        z = std::min(std::max(z * 1024.0f, 0.0f), 1023.0f);
        unsigned int xx = expandBits((unsigned int) x);
        unsigned int yy = expandBits((unsigned int) y);
        unsigned int zz = expandBits((unsigned int) z);
        return (xx << 2) + (yy << 1) + zz;
    }
}

class Sampler {
public:
    Sampler() = default;

    float get1D() { return dis(engine); }

    Vec2f get2D() { return {dis(engine), dis(engine)}; }

    void setSeed(int i) { engine.seed(i); }

private:
    std::default_random_engine engine;
    std::uniform_real_distribution<float> dis;
};


#endif //UTILS_H_
