#ifndef __TEXT_LABEL_HEADER__
#define __TEXT_LABEL_HEADER__
#define STB_TRUETYPE_IMPLEMENTATION

#include <string>
#include "../Dependancies/glad/glad.h"
#include "../Dependancies/glm/glm.hpp"
#include "../Dependancies/GLFW/glfw3.h"
#include "../Dependancies/stb/stb_truetype.h"

class TextLabel
{
    public:
        TextLabel();
        void CreateLabel(std::string LabelContents,std::string Text,glm::vec2 TextPosition);
        void RenderLabel();
    private:
        glm::vec2       Position;
        stbtt_fontinfo  NewFont;
        GLuint          FontTexture;
        unsigned char  *FontBitmap;
        unsigned char  *TrueTypeBuffer;
        int             Width;
        int             Height;
        int             XOffset;
        int             YOffset;
};

#endif