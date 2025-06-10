#ifndef __WINDOW_HEADER__
#define __WINDOW_HEADER__

#include <vector>
#include "../Core/Frame.h"
#include "../Core/Label.h"

typedef struct
{
    Frame       FrameBackground;
    Frame       FrameForeground;
    Frame       FrameHeaderContainer;
    Frame       FrameHeaderTitle;
    Frame       FrameExitClose;
    Label       HeaderTitle;
} WindowStructure;

class Window
{
    public:
        Window();
        void            DestroyEngineWindow();
        void            CreateEngineWindow(int EngineDrawWidth,int EngineDrawHeight,std::string WindowVertexLocation,std::string WindowFragmentLocation,std::string WindowTitle);
        void            RenderEngineWindow();
        bool            IsWindowOpen = false;
        glm::vec3       FrameColour;
        glm::vec2       FramePosition;
        glm::vec2       FrameScale;
        WindowStructure NewWindowStructure;
};

#endif