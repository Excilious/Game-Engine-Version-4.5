#include "Viewport.h"

Viewport::Viewport() {}

void WeaponActivated(GLFWwindow* EngineWindow,int Button,int Action,int Mods)
{
    Viewport* EngineViewport = static_cast<Viewport*>(glfwGetWindowUserPointer(EngineWindow));
    if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS)
    {
        EngineViewport->FireWeapon = false;
        if (EngineViewport->FireWeapon) {return;}
        if (EngineViewport->WeaponDictionary[EngineViewport->WeaponIndex].CurrentAmmo-- < 0)    {EngineViewport->WeaponDictionary[EngineViewport->WeaponIndex].CurrentAmmo = 0; SysPrint("No more ammo",2); return;}
        EngineViewport->AudioSystem.PlayLocalSound("../../Fusion/Audio/ShotgunFire.wav");
        SysPrint("Fired with "+std::to_string(EngineViewport->WeaponDictionary[EngineViewport->WeaponIndex].CurrentAmmo)+" bullets.",2);

        if (glfwGetTime()-EngineViewport->WeaponDelay >= EngineViewport->WeaponDictionary[EngineViewport->WeaponIndex].ReloadDelay)
        {
            EngineViewport->FireWeapon    = true;
            EngineViewport->WeaponDelay   = glfwGetTime();
            EngineViewport->AudioSystem.PlayLocalSound("../../Fusion/Audio/ShotgunReload.wav");
        }
    }
}

void Viewport::CreateViewport(GLFWwindow* EngineWindow,Parser* EngineParser,nlohmann::json NewJson)
{
    this->ViewportJSON          = NewJson;
    for (int Index = 0; Index < this->ViewportJSON["Viewports"].size(); Index++)
    {
        glm::vec3 Scale     = glm::vec3(this->ViewportJSON["Viewports"][Index]["Scale"][0],this->ViewportJSON["Viewports"][Index]["Scale"][1],this->ViewportJSON["Viewports"][Index]["Scale"][2]);
        glm::quat Rotation  = glm::quat(this->ViewportJSON["Viewports"][Index]["Rotation"][0],this->ViewportJSON["Viewports"][Index]["Rotation"][1],this->ViewportJSON["Viewports"][Index]["Rotation"][2],this->ViewportJSON["Viewports"][Index]["Rotation"][3]);

        this->WeaponInstances.GenerateStaticEntity(EngineParser,
            this->ViewportJSON["Viewports"][Index]["ModelName"],
            this->ViewportJSON["Viewports"][Index]["MeshReference"],
            EngineParser->GetGlobalDatapackData(this->ViewportJSON["Viewports"][Index]["ShaderVert"]),
            EngineParser->GetGlobalDatapackData(this->ViewportJSON["Viewports"][Index]["ShaderFrag"]),
            glm::vec3(0.0f,-0.5f,-0.25f),Scale,Rotation,this->ViewportJSON["Viewports"][Index]["TextureReference"],
            glm::vec2(this->ViewportJSON["Viewports"][Index]["TextureScale"][0],this->ViewportJSON["Viewports"][Index]["TextureScale"][1])
        );

        WeaponInfomation NewWeapon;
        NewWeapon.CurrentAmmo   = this->ViewportJSON["Viewports"][Index]["MaxAmmo"][0];
        NewWeapon.FireDelay     = this->ViewportJSON["Viewports"][Index]["ShootDelay"][0];
        NewWeapon.MaxAmmo       = this->ViewportJSON["Viewports"][Index]["MaxAmmo"][0];
        NewWeapon.ReloadDelay   = this->ViewportJSON["Viewports"][Index]["ReloadDelay"][0];
        this->WeaponDictionary.insert({Index,NewWeapon});
    }
}

void Viewport::RenderViewport(GLFWwindow* EngineWindow,Camera* EngineCamera,float Near,float Far,float FieldOfView,float PlayerSpeed)
{
    glfwSetWindowUserPointer(EngineWindow,this);
    glfwSetMouseButtonCallback(EngineWindow,WeaponActivated);
    if (this->PlayerOldPosition != EngineCamera->Position) 
    {
        float SwayAmount = 0.03f;
        float SwayOffset = 0.0f;
        float BobbingSpeed = 10.0f;
        float BobbingAmount = 0.05f;
        float BobbingOffset = 0.0f;
        if (glm::length(PlayerSpeed) > 0.1f)
        {
            SwayOffset = cos(glfwGetTime()*BobbingSpeed)*SwayAmount;
            BobbingOffset = sin(glfwGetTime()*BobbingSpeed)*BobbingAmount;
        }
        this->WeaponInstances.EntityList[this->WeaponIndex].NewEntityInstance.TranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,SwayOffset-0.5f,BobbingOffset-0.25f));
        this->PlayerOldPosition = EngineCamera->Position;
    }
    this->WeaponInstances.EntityList[this->WeaponIndex].NewEntityInstance.DrawModel(
        &(this->WeaponInstances.EntityList[this->WeaponIndex].ShaderEntity),
        EngineCamera,Near,Far,FieldOfView,EngineCamera->ProjectionMatrix*glm::mat4(1.0f)
    );
}