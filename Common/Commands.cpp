#include "Commands.h"
#include "Console.h"

std::vector<Register> RegisterList = {
    {"cullface",    0},
    {"multisample", 1},
    {"depth",       1},
    {"blend",       1},
    {"vsyncactive", 1},
    {"showfps",     true},
    {"polygon",     false},
    {"exit",        false},
    {"speed",       4.2f},
    {"accel",       0.01f},
    {"decel",       0.0005f},
    {"frict",       15.6f},
    {"fov",         90.0f},
    {"near",        0.1f},
    {"far",         1000.0f},
    {"world",       "-1"},
    {"bsp",         "../../Fusion/Castle.bsp"},
};

std::map<std::string,unsigned int> GLMap =  {
    {"cullface",    GL_CULL_FACE},
    {"depth",       GL_DEPTH_TEST},
    {"blend",       GL_BLEND},
    {"multisample", GL_MULTISAMPLE}
};

std::variant<int,float,bool,unsigned int,std::string> GetFromRegister(std::string SearchKey)
{
    int Index;
    for (int RegisterIndex = 0; RegisterIndex < RegisterList.size(); RegisterIndex++)
    {if (RegisterList[RegisterIndex].Key == SearchKey) {Index = RegisterIndex;}}
    return RegisterList[Index].Value;
}

void SetFromRegister(std::string SetKey,std::variant<int,float,bool,unsigned int,std::string> SetValue)
{
    bool AlreadyFound = false;
    for (int RegisterIndex = 0; RegisterIndex < RegisterList.size(); RegisterIndex++)
    {if (RegisterList[RegisterIndex].Key == SetKey) {RegisterList[RegisterIndex].Value = SetValue; AlreadyFound = true;}}

    if (!AlreadyFound)
    {
        Register NewRegister;
        NewRegister.Key     = SetKey;
        NewRegister.Value   = SetValue;
        RegisterList.push_back(NewRegister);
    }
}

void EditRegisterConsole(std::string Command)
{
    if (Command.find("+set") != std::string::npos)
    {
        std::istringstream  StringStream(Command);
        std::string         Command,Key;
        int                 Value;
        StringStream >> Command >> Key >> Value;
    
        if (std::holds_alternative<bool>(GetFromRegister(Key)))                 {SetFromRegister(Key,((Value == 1) ? true : false));}
        else if (std::holds_alternative<int>(GetFromRegister(Key)))             {SetFromRegister(Key,Value);} 
        else if (std::holds_alternative<float>(GetFromRegister(Key)))           {SetFromRegister(Key,(float)Value);} 
        else if (std::holds_alternative<std::string>(GetFromRegister(Key)))     {SetFromRegister(Key,std::to_string(Value));} 
        SysPrint("Successfully set "+Key+" to "+std::to_string(Value),1);
    }
}

void PrintAllRegisters(std::string Command)
{
    for (int Index = 0; Index < RegisterList.size(); Index++)
    {
        if (std::holds_alternative<bool>(RegisterList[Index].Value))                {SysPrint(RegisterList[Index].Key+" = "+std::to_string(std::get<bool>(RegisterList[Index].Value))+" (bool)",1);}
        else if (std::holds_alternative<int>(RegisterList[Index].Value))            {SysPrint(RegisterList[Index].Key+" = "+std::to_string(std::get<int>(RegisterList[Index].Value))+" (int)",1);}
        else if (std::holds_alternative<float>(RegisterList[Index].Value))          {SysPrint(RegisterList[Index].Key+" = "+std::to_string(std::get<float>(RegisterList[Index].Value))+" (unsigned int)",1);}
        else if (std::holds_alternative<std::string>(RegisterList[Index].Value))    {SysPrint(RegisterList[Index].Key+" = "+std::get<std::string>(RegisterList[Index].Value)+" (string)",1);}
    }
}

void ExitGameFunction(std::string Command)
{
    SysPrint("Exiting Fusion...",1);
    SetFromRegister("exit",true);
}

void Clear(std::string Command)
{ConsoleContents = {};}

void Dump(std::string Command)
{
    try
    {
        std::ofstream NewFile("ConsoleLog.txt");
        for (int Index = 0; Index < ConsoleContents.size(); Index++) {NewFile << ConsoleContents[Index].TextContent << "\n";}
        NewFile.close();
        SysPrint("Successfully dumped logs to ConsoleLog.txt",1);
    }
    catch(const std::exception& Exception)
    {
        SysPrint("Failed to dump logs! Reason: "+(std::string)(Exception.what()),1);
    }
}

void SetVSync(std::string Command)
{
    if (std::get<bool>(GetFromRegister("vsyncactive"))) 
    {SetFromRegister("vsyncactive",!std::get<bool>(GetFromRegister("vsyncactive")));}
    glfwSwapInterval((std::get<bool>(GetFromRegister("vsyncactive")) == true) ? 1 : 0);
}

void UpdateVariables(std::string Command)
{
    SysPrint("Updating GLValues...",1);
    for (int RegisterIndex = 0; RegisterIndex < RegisterList.size(); RegisterIndex++)
    {
        if (GLMap.find(RegisterList[RegisterIndex].Key) != GLMap.end() && std::holds_alternative<int>(RegisterList[RegisterIndex].Value))
        {
            if (std::get<int>(RegisterList[RegisterIndex].Value) == 1)  {glEnable(GLMap.find(RegisterList[RegisterIndex].Key)->second); SysPrint("Enabled "+RegisterList[RegisterIndex].Key,1);}
            else                                                        {glDisable(GLMap.find(RegisterList[RegisterIndex].Key)->second); SysPrint("Disabled "+RegisterList[RegisterIndex].Key,1);}
        }        
    }
}