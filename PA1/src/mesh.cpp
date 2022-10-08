#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <vector>
#include <sstream>

Mesh::Mesh(const std::string &path) {
    loadDataFromFile(getPath(path));
//    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertices.size() * sizeof(Vertex)), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(indices.size() * sizeof(GLuint)), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 *
 *
 * For each line starting with 'v' contains 3 floats, which
 * represents the position of a vertex
 *
 * For each line starting with 'n' contains 3 floats, which
 * represents the normal of a vertex
 *
 * For each line starting with 'f' contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {
    /**
     * path stands for the where the object is storaged
     * */
    // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
    std::ifstream infile(path);
    std::string data;
    std::vector<vec3> V, N;
    while (std::getline(infile, data)) {
        std::stringstream ss(data);
        while (std::getline(ss, data, ' ')) {
            if (data == "#") {
                std::getline(ss, data);
            } else if (data == "v") {
                float v1, v2, v3;
                ss >> v1 >> v2 >> v3;
                vertices.push_back(Vertex{.position=vec3(v1, v2, v3)});
//                V.emplace_back(v1, v2, v3);
            } else if (data == "n") {
                float n1, n2, n3;
                ss >> n1 >> n2 >> n3;
                N.emplace_back(n1, n2, n3);
            } else if (data == "f") {
                GLuint v1, v2, v3, n1, n2, n3;
                ss >> v1 >> n1
                   >> v2 >> n2
                   >> v3 >> n3;
                vertices[v1].normal = N[n1];
                vertices[v2].normal = N[n2];
                vertices[v3].normal = N[n3];
                indices.push_back(v1);
                indices.push_back(v2);
                indices.push_back(v3);
            }
        }
    }
}

/**
 * TODO: implement your draw code here
 */
void Mesh::draw() const {
    // !DELETE THIS WHEN YOU FINISH THIS FUNCTION
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,(GLsizei)(indices.size()), GL_UNSIGNED_INT, nullptr);
}