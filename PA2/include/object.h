#ifndef _object_H_
#define _object_H_

#include "defines.h"
#include <shader.h>
#include <vector>

struct Vertex {
    vec3 position;
    vec3 normal;
};

struct DrawMode {
    GLenum primitive_mode;
};

class Object {
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    DrawMode draw_mode;

    Object() = default;

    ~Object() = default;

    void init();

    void drawArrays() const;

    void drawElements() const;

};

#endif