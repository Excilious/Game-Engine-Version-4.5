#ifndef __ENTITY_HEADER__
#define __ENTITY_HEADER__

#include "../Rendering/Model.h"

typedef enum
{
    ENT_STATIC,
    ENT_DYNAMIC
} EntityType;

class Entity : public Model
{   
    public:
        Entity();
        void CreateEntity(std::string EntityName,std::string ModelFile,Parser* NewParser,glm::vec3 Position,glm::quat Rotation,glm::vec3 Scale,std::string TextureReference,glm::vec2 TextureSize);
        void CreateShadowMap();
        float           ModelAmbience = 0.1;
        float           ModelSpecular = 0.5;
    private:
        unsigned int    ShadowMapFrameBuffer;
        unsigned int    ShadowMapWidth,ShadowMapHeight = 2048;
        unsigned int    ShadowMap;
        float           ClampColour[4] = {1.0f,1.0f,1.0f,1.0f};

};

#endif