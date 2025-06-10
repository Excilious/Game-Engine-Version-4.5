#ifndef __SHADER_HEADER_FILE__
#define __SHADER_HEADER_FILE__
#define GLM_ENABLE_EXPERIMENTAL

#define VERTEX_REPORT_ID        0x000001
#define FRAGMENT_REPORT_ID      0x000002
#define PROGRAM_REPORT_ID       0x000003

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

#include "../Dependancies/glad/glad.h"
#include "../Dependancies/GLFW/glfw3.h"
#include "../Dependancies/glm/gtx/vector_angle.hpp"

std::string GetShaderContents(const char* ShaderName);
void ReportErrors(unsigned int Shaders,int Type);

class Shader
{
    public:
        Shader();
        void    CreateNewShader(const char* VertexShaderFile,const char* FragmentShaderFile,bool Source);
        void    ActivateShader();
        void    SetVector4(glm::vec4 Vector,std::string Location);
        void    SetVector3(glm::vec3 Vector,std::string Location);
        void    SetVector2(glm::vec2 Vector,std::string Location);
        void    SetVector1(float Vector,std::string Location);
        void    SetMatrix4(glm::mat4 Matrix,std::string Location);

        void    DestroyShader();
        GLuint  Program;
};


#endif