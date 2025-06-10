#include "Audio.h"
#include "../Common/Console.h"

Audio::Audio()
{}

DWORD Audio::GetVolumeFromDistance(float distance, float maxDistance) {
    if (distance < 0) distance = 0;
    if (distance > maxDistance) distance = maxDistance;
    float volumeFactor = 1.0f - (distance / maxDistance);
    DWORD volume = static_cast<DWORD>(volumeFactor * 0xFFFF);

    return (volume << 16) | volume; 
}

void Audio::ReadMemory(std::string FileName, BYTE** PointerByte, DWORD *FileSize)
{
    std::ifstream File(FileName, std::ios::binary);
    if (!File.is_open()) {SysPrint("Failed to open file"+(std::string)FileName,2);return;}

    File.seekg(0, std::ios::end);
    int Limit       = File.tellg();
    *FileSize       = Limit;
    *PointerByte    = new BYTE[Limit];
    
    File.seekg(0, std::ios::beg);
    File.read((char*)*PointerByte, Limit);
    File.close();
}

void Audio::AdjustSoundFromVolume(glm::vec3 AbsoluteDistance,glm::vec3 Distance)
{
    Distance                = glm::abs(Distance);
    AbsoluteDistance        = glm::abs(AbsoluteDistance);
    float MagnitudeDistance = sqrt((Distance.x*Distance.x)+(Distance.y*Distance.y)+(Distance.z*Distance.z));
    float AbsoluteMagnitude = sqrt((AbsoluteDistance.x*AbsoluteDistance.x)+(AbsoluteDistance.y*AbsoluteDistance.y)+(AbsoluteDistance.z*AbsoluteDistance.z));

    waveOutSetVolume(0,0xFFFFF); 
}

void Audio::PlayLocalSound(std::string FileName)
{
    this->FileName  = FileName;
    PlaySoundA((LPCSTR)this->FileName.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}