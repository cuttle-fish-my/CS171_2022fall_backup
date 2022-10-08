#include <shader.h>
#include <utils.h>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
    if (gsPath == "default") {
        init(vsPath, fsPath);
    } else {
        init(vsPath, fsPath, gsPath);
    }
}

void Shader::init(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
    if (gsPath == "default") {
        initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath));
    } else {
        initWithCode(getCodeFromFile(vsPath), getCodeFromFile(fsPath), getCodeFromFile(gsPath));
    }
}

void Shader::initWithCode(const std::string &vs, const std::string &fs, const std::string &gs) {
    unsigned int vertexShader, fragmentShader, geometryShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vertexSource = vs.c_str();
    const char *fragmentSource = fs.c_str();

    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    GLint success;
    GLchar log[1000];

    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1000, nullptr, log);
        std::cout << "vertex shader get compile error:" << std::endl
                  << log << std::endl;
    }

    if (gs != "default") {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        const char *geometrySource = gs.c_str();
        glShaderSource(geometryShader, 1, &geometrySource, nullptr);

        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometryShader, 1000, nullptr, log);
            std::cout << "geometry shader get compile error:" << std::endl
                      << log << std::endl;
        }
    }


    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 1000, nullptr, log);
        std::cout << "fragment shader get compile error:" << std::endl
                  << log << std::endl;
    }
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    if (gs != "default") {
        glAttachShader(id, geometryShader);
    }
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 1000, nullptr, log);
        std::cout << "shader program get linked error:" << std::endl
                  << log << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (gs != "default") {
        glDeleteShader(geometryShader);
    }

}

std::string Shader::getCodeFromFile(const std::string &path) {
    if (path == "default") return "default";
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(getPath(path));
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    } catch (std::ifstream::failure &e) {
        LOG_ERR("File Error: " + std::string(e.what()));
    }
    return code;
}

void Shader::use() const { glUseProgram(id); }

GLint Shader::getUniformLocation(const std::string &name) const {
    return glGetUniformLocation(id, name.c_str());
}

void Shader::setInt(const std::string &name, GLint value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, GLfloat value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat3(const std::string &name, const mat3 &value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE,
                       glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                       glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const vec4 &value) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}
