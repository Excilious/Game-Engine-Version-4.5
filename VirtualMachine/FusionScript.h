#ifndef __FUSIONSCRIPT_HEADER__
#define __FUSIONSCRIPT_HEADER__

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#define INSTRUCTION_PUSH        0x0001
#define INSTRUCTION_POP         0x0002
#define INSTRUCTION_DEFINE      0x0003
#define INSTRUCTION_UNDEFINE    0x0004
#define INSTRUCTION_GOTO        0x0005
#define INSTRUCTION_NORET       0x0006
#define INSTRUCTION_PGOTO       0x0007
#define INSTRUCTION_ADD         0x0008
#define INSTRUCTION_MUL         0x0009
#define INSTRUCTION_DIV         0x0010
#define INSTRUCTION_SUB         0x0011
#define INSTRUCTION_RET         0x0012

typedef struct 
{
    std::string IntermediateOpcode;
    int         Opcode;
} InstructionType;

class FusionScript
{
    public:
        FusionScript();
        void                        ParseIntermediateCode(std::string FileDirectory);
        std::vector<std::string>    RemoveWhitespace(const std::string& CodeString);
    private:
        int                         ReturnSyntaxCasepoint(std::string Syntax);
        std::vector<std::vector<std::string>>   ParsedCode;
        std::vector<int>                        Stack;
        std::vector<int>                        Registers;
};

#endif