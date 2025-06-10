#include "Image.h"

Image::Image() {}

void Image::SetSceneTexture(unsigned char* Buffer)
{
    glGenTextures(1,&this->SceneTextureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->SceneTextureID);
    this->TextureID = this->SceneTextureID;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1024,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,Buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(Buffer);
    glBindTexture(GL_TEXTURE_2D,0);
}

void Image::CreateImage(unsigned char* ImageBuffer,glm::vec2 Position,glm::vec2 Scale,int Width,int Height,float Rotation,std::string FrameVertex,std::string FrameFragment,int ImageWidth,int ImageHeight)
{
    this->CreateFrame(Position,Scale,Width,Height,glm::vec3(0.0f,0.0f,0.0f),Rotation,FrameVertex,FrameFragment);
    glBindTexture(GL_TEXTURE_2D,0);
    this->SetSceneTexture(ImageBuffer);
    this->IsFrameImage = true;
    this->NewShader.ActivateShader();
}