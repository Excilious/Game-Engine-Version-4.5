#include "Console.h"

typedef struct CommandFunctions
{
    Function        ExecutableFunction;
    std::string     CommandString;
} Command;

std::vector<Label>          ConsoleContents = {};
std::vector<Command>        ConsoleLists;
Window                      ConsoleWindow;
Label                       StaticLabel;
std::string                 CommandString;
std::string                 ExtraText           = "";
float                       LastStaticTime      = 0.0f;;
float                       BackspaceLastTime   = 0.0f;
float                       EnterLastTime       = 0.0f;
bool                        ConsoleOpen         = false;

void SysPrint(std::string Text,int ErrorCode = 1)
{
    glm::vec3 Colour;
    if      (ErrorCode == 1)    {Colour = CONSOLE_INFO;}
    else if (ErrorCode == 2)    {Colour = CONSOLE_WARN;}
    else if (ErrorCode == 3)    {Colour = CONSOLE_ERROR;}

    Label NewTextLabel;
    NewTextLabel.CreateLabel(Text,glm::vec2(0.0f,0.0f),1.05f,Colour);
    std::cout << Text << "\n";
    ConsoleContents.push_back(NewTextLabel);
}

void CommandAddCommand(std::string NewCommandString,Function CommandFunction)
{
    Command NewCommand;
    for (int CommandIndex = 0; CommandIndex < ConsoleLists.size(); CommandIndex++)
    {if (ConsoleLists[CommandIndex].CommandString == NewCommandString) {SysPrint("Command already exists.",2); return;}}
    NewCommand.CommandString        = NewCommandString;
    NewCommand.ExecutableFunction   = CommandFunction;
    ConsoleLists.push_back(NewCommand);
}

void CommandGenerateBackFrame(int Width,int Height,std::string FrameVertex,std::string FrameFragment)
{
    CommandAddCommand("exit",ExitGameFunction);
    CommandAddCommand("-lstreg",PrintAllRegisters);
    CommandAddCommand("-update",UpdateVariables);
    CommandAddCommand("-dump",Dump);
    CommandAddCommand("-clear",Clear);
    //CommandAddCommand("-vsync",SetVSync);

    StaticLabel.CreateLabel("]",glm::vec2(0.0f,0.0f),1.05f,glm::vec3(1.0f,1.0f,1.0f));
    ConsoleWindow.CreateEngineWindow(Width,Height,FrameVertex,FrameFragment,"Console");
}

void CommandUpdateCommandExecution()
{
    int Index = -1;
    for (int NewIndex = 0; NewIndex < ConsoleLists.size(); NewIndex++)
    {
        if (ConsoleLists[NewIndex].CommandString == CommandString) {Index=NewIndex;}
    }
    if (Index != -1)                        {ConsoleLists[Index].ExecutableFunction(CommandString);}
    else if (CommandString.find("+set") != std::string::npos)   {EditRegisterConsole(CommandString);}
    else                                                        {SysPrint("Invalid/Unknown Command",2);}
    CommandString = "";
}

void CommandUpdateInput(GLFWwindow* NewWindow)
{
    float BackspaceCurrentTime  = glfwGetTime();
    float EnterCurrentTime      = glfwGetTime();

    if (ConsoleOpen)    {glfwSetInputMode(NewWindow,GLFW_CURSOR,GLFW_CURSOR_NORMAL);}
    else                {glfwSetInputMode(NewWindow,GLFW_CURSOR,GLFW_CURSOR_DISABLED);}

    if (glfwGetKey(NewWindow,GLFW_KEY_BACKSPACE) == GLFW_PRESS && (BackspaceCurrentTime-BackspaceLastTime) > 0.25f && CommandString.size() != 0) 
    {CommandString.erase(CommandString.size()-1); BackspaceLastTime = BackspaceCurrentTime;}
    if (glfwGetKey(NewWindow,GLFW_KEY_ENTER) == GLFW_PRESS && (EnterCurrentTime-EnterLastTime) > 0.25f && CommandString.size() != 0) 
    {CommandUpdateCommandExecution(); EnterLastTime = EnterCurrentTime;}
}

void CommandUpdateKeyInput(GLFWwindow* NewWindow,unsigned int KeyCode)
{if (ConsoleOpen) {CommandString += (char)KeyCode;}}

void CommandRenderText(GLFWwindow* NewWindow)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    if (!ConsoleOpen) {return;}
    float NewStaticTime = glfwGetTime();
    if (NewStaticTime-LastStaticTime > 0.5f)
    {
        if (ExtraText == "[") {ExtraText = "";} else {ExtraText = "[";}
        LastStaticTime = NewStaticTime;
    }

    glfwSetCharCallback(NewWindow,CommandUpdateKeyInput);
    CommandUpdateInput(NewWindow);
    StaticLabel.SetNewText("] "+CommandString+ExtraText);
    ConsoleWindow.RenderEngineWindow();

    glm::vec2 FramePos      = ConsoleWindow.NewWindowStructure.FrameForeground.Position;
    glm::vec2 FrameScale    = ConsoleWindow.NewWindowStructure.FrameForeground.Scale;

    int WindowWidth, WindowHeight;
    glfwGetFramebufferSize(NewWindow,&WindowWidth,&WindowHeight);

    float LineSpacing   = 18.0f;
    float BottomPadding = 20.0f;
    float StartY        = FramePos.y-FrameScale.y/2.0f+BottomPadding;
    float LineHeight    = ConsoleContents.empty() ? 20.0f : ConsoleContents[0].Scale + LineSpacing;
    int MaxVisibleLines = (int)((FrameScale.y-2.0f*BottomPadding)/LineHeight);
    int StartIndex      = std::max(0,(int)ConsoleContents.size()-MaxVisibleLines);

    for (int TextIndex = StartIndex;TextIndex < ConsoleContents.size();TextIndex++)
    {
        if (TextIndex > 18)
        {
            int RenderLine                          = TextIndex-StartIndex;
            float AxisY                             = StartY+RenderLine*LineHeight;
            ConsoleContents[TextIndex].TextPosition = glm::vec2(FramePos.x-FrameScale.x / 2.0f + 10.0f,AxisY);
            ConsoleContents[TextIndex].RenderText();
        }
    }

    float BottomY = ConsoleWindow.NewWindowStructure.FrameForeground.Position.y
    + ConsoleWindow.NewWindowStructure.FrameForeground.Scale.y / 2.25f
    + StaticLabel.Scale * 0.05f;

    float PositionX = ConsoleWindow.NewWindowStructure.FrameForeground.Position.x
    - ConsoleWindow.NewWindowStructure.FrameForeground.Scale.x / 2.0f
    + 10.0f;

    StaticLabel.TextPosition = glm::vec2(PositionX,BottomY);
    StaticLabel.RenderText();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void CommandDelete()
{    
    for (int TextLabelIndex = 0; TextLabelIndex < ConsoleContents.size(); TextLabelIndex++) {ConsoleContents[TextLabelIndex].DestroyText();}
    ConsoleWindow.DestroyEngineWindow();
}