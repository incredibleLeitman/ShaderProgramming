#pragma once
// taken from https://learnopengl.com/Getting-started/Shaders
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;

    Shader (const char* name) : ID(glCreateProgram())
    {
        unsigned int shader_id = 0;
        std::string base = name;
        std::string shader = base + ".vs";
        if (file_exists(shader.c_str()))
        {
            shader_id = load_shader(shader.c_str(), GL_VERTEX_SHADER);
            //std::cout << "attaching vertex shader " << shader << " with id " << shader_id << std::endl;
            glAttachShader(ID, shader_id);
            glDeleteShader(shader_id);
        }

        shader = base + ".fs";
        if (file_exists(shader.c_str()))
        {
            shader_id = load_shader(shader.c_str(), GL_FRAGMENT_SHADER);
            //std::cout << "attaching fragment shader " << shader << " with id " << shader_id << std::endl;
            glAttachShader(ID, shader_id);
            glDeleteShader(shader_id);
        }

        shader = base + ".gs";
        if (file_exists(shader.c_str()))
        {
            shader_id = load_shader(shader.c_str(), GL_GEOMETRY_SHADER);
            //std::cout << "attaching geometry shader " << shader << " with id " << shader_id << std::endl;
            glAttachShader(ID, shader_id);
            glDeleteShader(shader_id);
        }

        shader = base + ".tcs";
        if (file_exists(shader.c_str()))
        {
            shader_id = load_shader(shader.c_str(), GL_TESS_CONTROL_SHADER);
            //std::cout << "attaching tess control shader " << shader << " with id " << shader_id << std::endl;
            glAttachShader(ID, shader_id);
            glDeleteShader(shader_id);
        }

        shader = base + ".tes";
        if (file_exists(shader.c_str()))
        {
            shader_id = load_shader(shader.c_str(), GL_TESS_EVALUATION_SHADER);
            //std::cout << "attaching tess eval shader " << shader << " with id " << shader_id << std::endl;
            glAttachShader(ID, shader_id);
            glDeleteShader(shader_id);
        }

        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    /* only available using glew
    void setBuffer(const std::string& name, int buffer)
    {
        glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), buffer);
    }
    // ------------------------------------------------------------------------
    void setSampler(const std::string& name, int buffer)
    {
        glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), buffer);
    }*/

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    unsigned int load_shader(const char *path, GLuint type)
    {
        std::string code;
        std::ifstream file;

        file.open(path);
        std::stringstream shader_stream;
        shader_stream << file.rdbuf();
        file.close();

        code = shader_stream.str();
        const char* code_charptr = code.c_str();

        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &code_charptr, NULL);
        glCompileShader(shader);
        checkCompileErrors(shader, "GL_SHADERTYPE_" + std::to_string(type));

        return shader;
    }

    inline bool file_exists (const char *name)
    {
        struct stat buffer;
        return (stat(name, &buffer) == 0);
    }
};
#endif // SHADER_H