#ifndef __AUDIO_HEADER__
#define __AUDIO_HEADER__

#include <windows.h>
#include <iostream>
#include <fstream>
#include "../Dependancies/glad/glad.h"
#include "../Dependancies/GLFW/glfw3.h"
#include "../Dependancies/glm/glm.hpp"

class Audio
{
    public:
        Audio();
        DWORD   GetVolumeFromDistance(float distance, float maxDistance);
        void    AdjustSoundFromVolume(glm::vec3 AbsoluteDistance,glm::vec3 Distance);
        void    ReadMemory(std::string FileName, BYTE** PointerByte, DWORD *FileSize);
        void    PlayLocalSound(std::string FileName);
    private:
        std::string FileName;
};

#endif