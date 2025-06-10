#ifndef __VIEWPORT_HEADER__
#define __VIEWPORT_HEADER__

#include <map>
#include "../Common/Parser.h"
#include "../Rendering/Camera.h"
#include "../World/Instance.h"
#include "../Audio/Audio.h"

struct WeaponInfomation
{
    int     MaxAmmo;
    int     CurrentAmmo;
    float   ReloadDelay;
    float   FireDelay;
};

class Viewport
{
    public:
        Viewport();
        void CreateViewport(GLFWwindow* EngineWindow,Parser* EngineParser,nlohmann::json NewJson);
        void RenderViewport(GLFWwindow* EngineWindow,Camera* EngineCamera,float Near,float Far,float FieldOfView,float PlayerSpeed);
        float                               WeaponDelay;
        float                               FireWait;
        bool                                FireWeapon              = false;
        int                                 WeaponIndex             = 0;
        std::map<int,WeaponInfomation>      WeaponDictionary;
        Audio                               AudioSystem;
    private:
        int                                 ZAxisMovement           = -0.25;
        glm::vec3                           PlayerOldPosition       = glm::vec3(0.0f,0.0f,0.0f);
        Instance                            WeaponInstances;
        nlohmann::json                      ViewportJSON;
};

#endif