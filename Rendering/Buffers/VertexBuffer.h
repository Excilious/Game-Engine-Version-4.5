#ifndef __VERTEX_BUFFER_HEADER__
#define __VERTEX_BUFFER_HEADER__

#include "../Dependancies/glad/glad.h"
#include "../Dependancies/glm/glm.hpp"
#include <vector>

struct SceneVertex
{
    glm::vec3 Positions;
    glm::vec3 Normals;
    glm::vec3 Colour;
    glm::vec2 TextureUV;
};

class VertexBufferObject
{
    public:
        GLuint VertexBuffer;
        VertexBufferObject(std::vector<SceneVertex>& Vertices);
        VertexBufferObject(std::vector<float>& Vertices);
        VertexBufferObject(std::vector<glm::vec3>& Vertices);
        void Bind();
        void Unbind();
        void Delete();
};

#endif
