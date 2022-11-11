#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <iostream>

#include "texture.h"

Texture::Texture(const std::string &path, Texture::Type t) {
    loadTextureFromFile(path);
    type = t;
}

void Texture::loadTextureFromFile(const std::string &path) {
    stbi_uc *img = stbi_load(path.c_str(), &length, &width, &channel, 0);
    if (!img) {
        std::cout << "failed to load image from path:" << path << std::endl;
        exit(1);
    }
    tex_data.assign(img, img + length * width * channel);
    stbi_image_free(img);
}

Vec3f Texture::getColor(int x, int y) const {
    if (channel == 1) {
        return Vec3f{
                (float) tex_data.at(x + y * length),
                (float) tex_data.at(x + y * length),
                (float) tex_data.at(x + y * length)
        } / 255.0f;
    }
    if (channel == 3) {
//        std::cout << x << " " << y << std::endl;
        return Vec3f{
                (float) tex_data.at(channel * (x + y * length) + 0),
                (float) tex_data.at(channel * (x + y * length) + 1),
                (float) tex_data.at(channel * (x + y * length) + 2)
        } / 255.0f;
    } else {
        std::cout << "invalid channel:" << channel << std::endl;
        exit(4);
    }
}

int Texture::getLength() const {
    return length;
}

int Texture::getWidth() const {
    return width;
}

int Texture::getChannel() const {
    return channel;
}

bool Texture::isInitialized() const {
    return (!tex_data.empty());
}


