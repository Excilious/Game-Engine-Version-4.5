#include "Player.h"
#include "../Common/Console.h"

Player::Player() {}

void Player::LoadPlayer(Parser* EngineParser, Camera* EngineCamera) {
    this->CreateActor("PlayerHumanoid","Ranger.gltf", EngineParser, 
        glm::vec3(0.0f, 0.0f, 0.0f),glm::quat(0.0f, 0.0f, 0.0f, 0.0f),glm::vec3(0.02f, 0.03f, 0.02f),
        "RangerTex.imgbuf",glm::vec2(256,256)
    );

    this->PlayerCharacterShader.CreateNewShader(
        EngineParser->GetGlobalDatapackData("Default.vert").c_str(),
        EngineParser->GetGlobalDatapackData("Default.frag").c_str(),
        true);
    this->GetAllAnimationInstances();
}

void Player::Movement(GLFWwindow* InputWindow, glm::vec3 Orientation, glm::vec3 Position) 
{
    if (ConsoleOpen) {return;}
    glfwSetInputMode(InputWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glm::vec3 NewMovement   = glm::vec3(0.0f,0.0f,0.0f);
    float MoveSpeed         = this->PlayerSpeed * 0.2f;

    if (glfwGetKey(InputWindow, GLFW_KEY_W) == GLFW_PRESS) {NewMovement += MoveSpeed * Orientation;}
    if (glfwGetKey(InputWindow, GLFW_KEY_A) == GLFW_PRESS) {NewMovement += MoveSpeed * -glm::normalize(glm::cross(Orientation, glm::vec3(0.0f, 1.0f, 0.0f)));}
    if (glfwGetKey(InputWindow, GLFW_KEY_S) == GLFW_PRESS) {NewMovement += MoveSpeed * -Orientation;}
    if (glfwGetKey(InputWindow, GLFW_KEY_D) == GLFW_PRESS) {NewMovement += MoveSpeed * glm::normalize(glm::cross(Orientation, glm::vec3(0.0f, 1.0f, 0.0f)));}
    if (glfwGetKey(InputWindow, GLFW_KEY_SPACE) == GLFW_PRESS && this->IsPlayerOnGround) {this->PlayerVelocity.y += 4.0f;this->IsPlayerOnGround = false;}
    this->PlayerPosition += NewMovement;
}

void Player::Destroy() {
    this->PlayerCharacterShader.DestroyShader();
    this->DestroyModel();
}
