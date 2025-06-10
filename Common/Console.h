#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#define CONSOLE_ERROR   glm::vec3(1.0f,0.0f,0.0f)
#define CONSOLE_WARN    glm::vec3(1.0f,1.0f,0.0f)
#define CONSOLE_INFO    glm::vec3(1.0f,1.0f,1.0f)

#include <windows.h>
#include "../Rendering/Interface/Core/Label.h"
#include "../Rendering/Interface/Core/Image.h"
#include "../Rendering/Interface/Elements/Window.h"
#include "../Dependancies/GLFW/glfw3.h"
#include "Commands.h"

typedef void (*Function) (std::string Command);
void CommandAddCommand(std::string NewCommandString,Function CommandFunction);
void CommandUpdateCommandExecution();

extern bool ConsoleOpen;
extern std::vector<Label> ConsoleContents;
extern void SysPrint(std::string Text,int ErrorCode);
extern void CommandUpdateInput(GLFWwindow* NewWindow);
extern void CommandRenderText(GLFWwindow* NewWindow);
extern void CommandGenerateBackFrame(int Width,int Height,std::string FrameVertex,std::string FrameFragment);
extern void CommandUpdateKeyInput(GLFWwindow* NewWindow,unsigned int KeyCode);
extern void CommandDelete();

#endif