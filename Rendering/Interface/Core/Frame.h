#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#define GLT_IMPLEMENTATION

#include <vector>
#include <map>
#include <string>

#include "../Dependancies/glad/glad.h"
#include "../Dependancies/glm/glm.hpp"
#include "../Dependancies/GLFW/glfw3.h"
#include "../Dependancies/gltext/gltext.h"

#include "../Dependancies/glm/gtc/matrix_transform.hpp"
#include "../Dependancies/glm/gtc/type_ptr.hpp"
#include "../../../Common/Shader.h"

class Frame
{
    public:
        Frame();
        void CreateFrame(glm::vec2 Position,glm::vec2 Scale,int Width,int Height,glm::vec3 Colour,float Rotation,std::string FrameVertex,std::string FrameFragment);
        void AddGradient(glm::vec3 Colour1,glm::vec3 Colour2);
        void AddBevelCorners(float Radius);
        void AddTransparancy(float Transparency);
        void RenderInstance();
        void DestroyInstance();

        float               WindowWidth;
        float               WindowHeight;
        float               Rotation;
        float               Transparency;
        float               Radius;
        float               GradientActive;
        float               CornersActive;
        float               TransparencyActive;
        bool                Active;
        
        glm::vec2           Position;
        glm::vec2           Scale;
        glm::vec3           Colour;
        glm::vec3           ColourGradient1;
        glm::vec3           ColourGradient2;
        glm::mat4           OrthoMatrix;
        std::string         FileName;
        Shader              NewShader;
        unsigned int        TextureID;
        bool                IsFrameImage = false;

    private:    
        glm::mat4           TranslationMatrix;
        glm::mat4           RotationMatrix;
        glm::mat4           ScaleMatrix;
        unsigned int        FrameVertexArray;
        unsigned int        FrameVertexBuffer;
        glm::mat4           ProjectionMatrix;
        glm::mat4           ModelMatrix;
};

#endif