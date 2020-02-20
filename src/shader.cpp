#include "shader.h"

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string parseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    std::string line;
    std::string shader = "";
    while (getline(stream, line))
    {
        shader += line + '\n';
    }

    stream.close();
    return shader;
}

unsigned int compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length);
        glGetShaderInfoLog(id, length, &length, &message[0]);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
        for (std::vector<char>::const_iterator i = message.begin(); i != message.end(); i++)
            std::cout << *i;
        std::cout << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}