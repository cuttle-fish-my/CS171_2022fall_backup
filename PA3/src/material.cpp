#include "material.h"
#include "image.h"
#include "stb_image.h"
#include <utility>
#include <iostream>

ConstColorMat::ConstColorMat() : diffuse_color({1, 1, 1}),
                                 specular_color({1, 1, 1}),
                                 ambient_color(1, 1, 1),
                                 shininess(16.f) {}

ConstColorMat::ConstColorMat(const Vec3f &color, float sh)
        : diffuse_color(color), specular_color(color), ambient_color(color), shininess(sh) {}

ConstColorMat::ConstColorMat(Vec3f diff, Vec3f spec, Vec3f ambient, float sh)
        : diffuse_color(std::move(diff)),
          specular_color(std::move(spec)),
          ambient_color(std::move(ambient)),
          shininess(sh) {}

InteractionPhongLightingModel ConstColorMat::evaluate(Interaction &interaction) const {
    InteractionPhongLightingModel m;
    // TODO: Your code here.
    m.ambient = ambient_color;
    m.diffusion = diffuse_color;
    m.shininess = shininess;
    m.specular = specular_color;
    return m;
}


TextureMat::TextureMat(const std::string &diffPath, const std::string &normPath, const std::string &dispPath,
                       float sh) {
    diffuse_map = Texture(diffPath, Texture::Type::DIFF);
    if (normPath != "none") {
        normal_map = Texture(normPath, Texture::Type::NORM);
    }
    if (dispPath != "none") {
        displace_map = Texture(dispPath, Texture::Type::DISP);
    }
    shininess = sh;
    length = diffuse_map.getLength();
    width = diffuse_map.getWidth();
    channel = diffuse_map.getChannel();

}


InteractionPhongLightingModel TextureMat::evaluate(Interaction &interaction) const {
    InteractionPhongLightingModel m;
    float u = interaction.uv.x();
    float v = interaction.uv.y();
    int x = std::min((int) (u * (float) length), length - 1);
    int y = std::min((int) (v * (float) width), width - 1);
    m.diffusion = diffuse_map.getColor(x, y);
    m.specular = m.diffusion;
    m.ambient = m.diffusion;
    m.shininess = shininess;
    if (normal_map.isInitialized()) {
        interaction.normal = (normal_map.getColor(x, y) * 2 - Vec3f{1.f, 1.f, 1.f}).normalized();
    }
    if (displace_map.isInitialized()) {
//        interaction.pos = Vec3f{1.f, 1.f, 1.f} - (displace_map.getColor(x, y));
        interaction.pos = displace_map.getColor(x, y);
    }
    return m;
}


