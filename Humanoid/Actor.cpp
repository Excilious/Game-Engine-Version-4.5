#include "Actor.h"

Actor::Actor()
{}

void Actor::CreateActor(std::string ActorName,std::string ModelFile,Parser* NewParser,glm::vec3 Position,glm::quat Rotation,glm::vec3 Scale,std::string TextureReference,glm::vec2 TextureSize)
{
    this->CreateEntity(ActorName,ModelFile,NewParser,Position,Rotation,Scale,TextureReference,TextureSize);
}

void Actor::GetAllAnimationInstances()
{
    for (int Index = 0; Index < this->MeshJson["animations"].size(); Index++)
    {
        std::cout << this->MeshJson["animations"][Index]["name"] << "\n";
    }
}  
