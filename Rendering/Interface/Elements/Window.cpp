#include "Window.h"

Window::Window()
{}

void Window::CreateEngineWindow(int EngineDrawWidth,int EngineDrawHeight,std::string WindowVertexLocation,std::string WindowFragmentLocation,std::string WindowTitle)
{
    this->NewWindowStructure.FrameBackground.CreateFrame(glm::vec2(600.0f,500.0f),glm::vec2(750.0f,450.0f),EngineDrawWidth,EngineDrawHeight,glm::vec3(72.0f,72.0f,72.0f),0.0f,WindowVertexLocation,WindowFragmentLocation);
    this->NewWindowStructure.FrameForeground.CreateFrame(glm::vec2(600.0f,520.0f),glm::vec2(725.0f,385.0f),EngineDrawWidth,EngineDrawHeight,glm::vec3(28.0f,28.0f,28.0f),0.0f,WindowVertexLocation,WindowFragmentLocation);
    this->NewWindowStructure.FrameHeaderTitle.CreateFrame(glm::vec2(600.0f,295.0f),glm::vec2(750.0f,40.0f),EngineDrawWidth,EngineDrawHeight,glm::vec3(28.0f,28.0f,28.0f),0.0f,WindowVertexLocation,WindowFragmentLocation);
    this->NewWindowStructure.FrameHeaderContainer.CreateFrame(glm::vec2(580.0f,290.0f),glm::vec2(680.0f,30.0f),EngineDrawWidth,EngineDrawHeight,glm::vec3(28.0f,28.0f,28.0f),0.0f,WindowVertexLocation,WindowFragmentLocation);
    this->NewWindowStructure.FrameBackground.AddGradient(glm::vec3(42.0f),glm::vec3(22.0f));
    this->NewWindowStructure.FrameForeground.AddGradient(glm::vec3(26.0f),glm::vec3(16.0f));
    this->NewWindowStructure.FrameHeaderTitle.AddGradient(glm::vec3(28.0f),glm::vec3(12.0f));
    this->NewWindowStructure.FrameHeaderContainer.AddGradient(glm::vec3(72.0f),glm::vec3(52.0f));
    this->NewWindowStructure.FrameHeaderContainer.AddBevelCorners(2);
    this->NewWindowStructure.HeaderTitle.CreateLabel(WindowTitle,glm::vec2(250.0f,280.0f),1.25f,glm::vec3(1.0f));
}

void Window::RenderEngineWindow()
{
    this->NewWindowStructure.FrameBackground.RenderInstance();
    this->NewWindowStructure.FrameForeground.RenderInstance();
    this->NewWindowStructure.FrameHeaderTitle.RenderInstance();
    this->NewWindowStructure.FrameHeaderContainer.RenderInstance();
    this->NewWindowStructure.HeaderTitle.RenderText();
}

void Window::DestroyEngineWindow()
{
    this->NewWindowStructure.FrameBackground.DestroyInstance();
    this->NewWindowStructure.FrameForeground.DestroyInstance();
    this->NewWindowStructure.FrameHeaderContainer.DestroyInstance();
    this->NewWindowStructure.FrameHeaderTitle.DestroyInstance();
    this->NewWindowStructure.HeaderTitle.DestroyText();
}