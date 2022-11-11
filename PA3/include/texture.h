#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_

#include <vector>
#include <string>

#include "core.h"

class Texture {
public:
    enum Type {
        DIFF, NORM, DISP
    };
    Type type{DIFF};

    Texture() = default;

    Texture(const std::string &path, Type t);

    void loadTextureFromFile(const std::string &path);

    [[nodiscard]] Vec3f getColor(int x, int y) const;

    int getLength() const;

    int getWidth() const;

    int getChannel() const;

    bool isInitialized() const;

private:
    std::vector<uint8_t> tex_data{};
    int length{};
    int width{};
    int channel{};
};

#endif //CS171_HW3_INCLUDE_TEXTURE_H_
