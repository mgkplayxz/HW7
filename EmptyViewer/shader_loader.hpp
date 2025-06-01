#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

GLuint LoadShader(const char* vertexPath, const char* fragmentPath) {
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);

    if (!vFile.is_open() || !fFile.is_open()) {
        std::cerr << "Failed to open shader files." << std::endl;
        return 0;
    }

    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    std::string vCode = vStream.str();
    std::string fCode = fStream.str();
    const char* vShaderCode = vCode.c_str();
    const char* fShaderCode = fCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    GLint success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr << "Vertex Shader Compilation Failed:\n" << infoLog << std::endl;
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cerr << "Fragment Shader Compilation Failed:\n" << infoLog << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

#endif
