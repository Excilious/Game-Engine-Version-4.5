#ifndef __TEXTURE_HEADER__
#define __TEXTURE_HEADER__

#include "../Dependancies/glad/glad.h"
#include "../Dependancies/stb/stb_image.h"
#include "../Common/Shader.h"

class Texture
{
    public: 
        Texture();
        void CreateBufferTexture(unsigned char* BufferData,unsigned int Width = 1024,unsigned int Height = 1024);
        void TextureUnit(Shader* NewShader,const char* Uniform,GLuint Unit);
        void CreateNormalTexture(const char* Image,std::string TextureType);
        void Bind();
        void Unbind();
        void Delete();

        unsigned int GetTextureFromFile(std::string Path,bool Gamma);
        GLuint      TextureID;
        std::string Type = "diffuse";
    private:
        int TextureIndexSlot = 0;
};

#endif 