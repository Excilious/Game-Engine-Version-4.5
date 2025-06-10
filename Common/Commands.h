#ifndef __COMMANDS_HEADER__
#define __COMMANDS_HEADER__

#include "../../Dependancies/glad/glad.h"
#include "../../Dependancies/GLFW/glfw3.h"
#include "../../Dependancies/glm/glm.hpp"
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <variant>

typedef struct
{
    std::string                                             Key;
    std::variant<int,float,bool,unsigned int,std::string>   Value;
} Register;

extern void ExitGameFunction(std::string Command);
extern void UpdateVariables(std::string Command);
extern void SetVSync(std::string Command);
extern void Dump(std::string Command);
extern void Clear(std::string Command);

extern void EditRegisterConsole(std::string Command);
extern void PrintAllRegisters(std::string Command);
extern std::vector<Register> RegisterList;

extern std::variant<int,float,bool,unsigned int,std::string> GetFromRegister(std::string SearchKey);
extern void SetFromRegister(std::string SetKey,std::variant<int,float,bool,unsigned int,std::string> SetValue);

#endif