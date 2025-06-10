#ifndef __ACTOR_HEADER__
#define __ACTOR_HEADER__

#include "../Dynamic/Entity.h"

class Actor: public Entity
{
    public:
        Actor();
        void CreateActor(std::string ActorName,std::string ModelFile,Parser* NewParser,glm::vec3 Position,glm::quat Rotation,glm::vec3 Scale,std::string TextureReference,glm::vec2 TextureSize);
        void GetAllAnimationInstances();
    private:
};

#endif