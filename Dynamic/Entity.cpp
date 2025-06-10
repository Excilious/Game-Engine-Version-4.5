#include "Entity.h"

Entity::Entity()
{}

void Entity::CreateShadowMap()
{
    glGenFramebuffers(1,&this->ShadowMapFrameBuffer);
    glGenTextures(1,&this->ShadowMap);
    glBindTexture(GL_TEXTURE_2D,this->ShadowMap);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,this->ShadowMapWidth,this->ShadowMapHeight,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,this->ClampColour);
    glBindFramebuffer(GL_FRAMEBUFFER,this->ShadowMapFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,this->ShadowMap,0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Entity::CreateEntity(std::string EntityName,std::string ModelFile,Parser* NewParser,glm::vec3 Position,glm::quat Rotation,glm::vec3 Scale,std::string TextureReference,glm::vec2 TextureSize)
{
    this->LoadFromGLTF(NewParser,ModelFile,Position,Rotation,Scale,TextureReference,TextureSize);
}

