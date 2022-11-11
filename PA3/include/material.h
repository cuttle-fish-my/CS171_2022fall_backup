#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_

#include "interaction.h"
#include "texture.h"

class Material {
public:
    Material() = default;

    virtual ~Material() = default;

    [[nodiscard]] virtual InteractionPhongLightingModel evaluate(Interaction &interaction) const = 0;

//    virtual bool isInitialized(Texture::Type) = 0;
};

class ConstColorMat : public Material {
public:
    ConstColorMat();

    explicit ConstColorMat(const Vec3f &color, float sh = 16.f);

    ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh = 16.f);

    [[nodiscard]] InteractionPhongLightingModel evaluate(Interaction &interaction) const override;

private:
    Vec3f diffuse_color;
    Vec3f specular_color;
    Vec3f ambient_color;
    float shininess;
};

class TextureMat : public Material {
public:
    TextureMat() = delete;

    explicit TextureMat(const std::string &diffPath, const std::string &normPath = "none",
                        const std::string &dispPath = "none", float sh = 16.f);

    InteractionPhongLightingModel evaluate(Interaction &interaction) const override;

//    bool isInitialized(Texture::Type type) override;

private:
    Texture diffuse_map{};
    Texture normal_map{};
    Texture displace_map{};
    int length;
    int width;
    int channel;
    float shininess;


};

#endif //CS171_HW3_INCLUDE_MATERIAL_H_
