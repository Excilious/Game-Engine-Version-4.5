#include "TextLabel.h"

TextLabel::TextLabel()
{}

void TextLabel::CreateLabel(std::string LabelContents,std::string Text,glm::vec2 TextPosition)
{
    this->Position          = TextPosition;
    this->TrueTypeBuffer    = new unsigned char[1<<20];
    stbtt_InitFont(&this->NewFont,this->TrueTypeBuffer,stbtt_GetFontOffsetForIndex(this->TrueTypeBuffer,0));
    this->FontBitmap = stbtt_GetCodepointBitmap(&this->NewFont,0,stbtt_ScaleForPixelHeight(&this->NewFont,32),'A',&this->Width,&this->Height,&this->XOffset,&this->YOffset);

    glGenTextures(1,&this->FontTexture);
    glBindTexture(GL_TEXTURE_2D,this->FontTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,this->Width,this->Height,0,GL_RED,GL_UNSIGNED_BYTE,this->FontBitmap);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void TextLabel::RenderLabel()
{
    float Vertices[] = {
        this->Position.x,this->Position.y,0.0f,0.0f,
        this->Position.x+this->Width,this->Position.y,1.0f,0.0f,
        this->Position.x+this->Width,this->Position.y+this->Height,1.0f,1.0f,
        this->Position.x,this->Position.y+this->Height,0.0f,1.0f
    };

    glBindTexture(GL_TEXTURE_2D,this->FontTexture);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
}