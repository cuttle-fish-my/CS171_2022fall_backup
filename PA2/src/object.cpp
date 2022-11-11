#include <object.h>
#include <shader.h>
#include <utils.h>

void Object::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (indices.size() * sizeof(GLuint)), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


//    std::vector<vec3> points;
//    for (auto v: vertices) {
//        points.push_back(v.position);
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (points.size() * sizeof(vec3)), &points[0], GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) (0 * sizeof(float)));
//    glEnableVertexAttribArray(0);


}


void Object::drawArrays() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) vertices.size());
    glBindVertexArray(0);
}


void Object::drawElements() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) (indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
