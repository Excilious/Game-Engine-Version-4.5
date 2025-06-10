#include "Frame.h"
#include "../../../Common/Console.h"

Frame::Frame() {}

void Frame::CreateFrame(glm::vec2 Position,glm::vec2 Scale,int Width,int Height,glm::vec3 Colour,float Rotation,std::string FrameVertex,std::string FrameFragment)
{
    float Vertices[] = { 
        -0.5f,  0.5f,   0.0f, 0.0f,
         0.5f, -0.5f,   0.0f, 0.0f, 
        -0.5f, -0.5f,   0.0f, 0.0f,
        -0.5f,  0.5f,   0.0f, 0.0f,
         0.5f,  0.5f,   0.0f, 0.0f, 
         0.5f, -0.5f,   0.0f, 0.0f  
    };

    this->Rotation          = Rotation;
    this->Scale             = Scale;
    this->Position          = Position;
    this->Colour            = glm::vec3(Colour.x/255.0f,Colour.y/255.0f,Colour.z/255.0f);
    this->WindowWidth       = Width;
    this->WindowHeight      = Height;
    this->GradientActive    = 0;
    this->NewShader.SetVector1(0.0f,"IsImage");
    this->NewShader.CreateNewShader(FrameVertex.c_str(), FrameFragment.c_str(), true);

    this->TransparencyActive    = 0;
    this->CornersActive         = 0;
    this->GradientActive        = 0;

    glGenVertexArrays(1, &this->FrameVertexArray);
    glGenBuffers(1, &this->FrameVertexBuffer);

    glBindVertexArray(this->FrameVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, this->FrameVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Frame::AddGradient(glm::vec3 Colour1,glm::vec3 Colour2)
{
    this->GradientActive    = 1;
    this->ColourGradient1   = glm::vec3(Colour1.x/255,Colour1.y/255,Colour1.z/255);
    this->ColourGradient2   = glm::vec3(Colour2.x/255,Colour2.y/255,Colour2.z/255);
}

void Frame::AddBevelCorners(float Radius)
{
    this->CornersActive     = 1;
    this->Radius            = Radius;
}

void Frame::AddTransparancy(float Transparency)
{
    this->TransparencyActive    = 1;
    this->Transparency          = Transparency;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void Frame::RenderInstance()
{
    this->ModelMatrix       = glm::mat4(1.0f);
    this->ModelMatrix       = glm::translate(this->ModelMatrix, glm::vec3(this->Position, 0.0f));
    this->ModelMatrix       = glm::rotate(this->ModelMatrix, glm::radians(this->Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    this->ModelMatrix       = glm::scale(this->ModelMatrix, glm::vec3(this->Scale, 1.0f));
    this->ProjectionMatrix  = glm::ortho(0.0f, (float)this->WindowWidth, (float)this->WindowHeight, 0.0f, -1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,this->TextureID);
    this->NewShader.ActivateShader();
    this->NewShader.SetMatrix4(this->ModelMatrix,"model");
    this->NewShader.SetMatrix4(this->ProjectionMatrix,"projection");
    this->NewShader.SetVector3(this->Colour,"spriteColor");

    if (this->GradientActive == 1)
    {
        this->NewShader.SetVector1((float)this->GradientActive,"Gradient");
        this->NewShader.SetVector3(this->ColourGradient1,"ColourA");
        this->NewShader.SetVector3(this->ColourGradient2,"ColourB");
    }
    if (this->CornersActive == 1)
    {
        this->NewShader.SetVector1((float)this->CornersActive,"Corner");
        this->NewShader.SetVector1(this->Radius,"Radius");
        this->NewShader.SetVector2(this->Scale,"scale");
    }
    if (this->TransparencyActive == 1)
    {
        this->NewShader.SetVector1((float)this->TransparencyActive,"IsTransparancy");
        this->NewShader.SetVector1(this->Transparency,"Transparency");
    }
    if (this->IsFrameImage)
    {
        this->NewShader.SetVector1(1.0f,"IsImage");
        this->NewShader.SetVector1(this->TextureID,"TextureID");
    }

    glBindVertexArray(this->FrameVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Frame::DestroyInstance()
{
    glDeleteVertexArrays(1, &this->FrameVertexArray);
    glDeleteBuffers(1, &this->FrameVertexBuffer);
    this->NewShader.DestroyShader();
}
