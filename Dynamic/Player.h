#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__

#include "../Humanoid/Actor.h"
#include "../Common/Parser.h"
#include "../Rendering/Lighting.h"

class Player: public Actor
{
    public:
        Player();
        void        LoadPlayer(Parser* EngineParser,Camera* EngineCamera);
        void        Movement(GLFWwindow* InputWindow,glm::vec3 Orientation,glm::vec3 Position);
        void        Destroy();

        float       PlayerSpeed         = 0.2f;
        float       PlayerAcceleration  = 0.01f;
        float       PlayerFriction      = 15.6f;
        float       PlayerDeceleration  = 0.0005f;
        glm::vec3   PlayerPosition;
        glm::vec3   PlayerVelocity;
    private:
        Shader      PlayerCharacterShader;
        bool        IsPlayerOnGround    = false;
        glm::quat   PlayerRotation      = glm::quat(0.0f,0.0f,0.0f,1.0f);
        glm::vec3   PlayerScale         = glm::vec3(3.0f,6.0f,3.0f);
        glm::vec3   PlayerOrientation;
};

#endif