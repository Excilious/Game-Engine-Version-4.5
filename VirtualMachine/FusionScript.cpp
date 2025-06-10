#include "FusionScript.h"

std::vector<InstructionType> InstructionSet = {
    {"Ins_0x0001",INSTRUCTION_PUSH},
    {"Ins_0x0002",INSTRUCTION_POP},
    {"Ins_0x0003",INSTRUCTION_DEFINE},
    {"Ins_0x0004",INSTRUCTION_UNDEFINE},
    {"Ins_0x0005",INSTRUCTION_GOTO},
    {"Lbl_0x0006",INSTRUCTION_NORET},
    {"Ins_0x0007",INSTRUCTION_PGOTO},
    {"Add_0x0008",INSTRUCTION_ADD},
    {"Mul_0x0009",INSTRUCTION_MUL},
    {"Div_0x0010",INSTRUCTION_DIV},
    {"Sub_0x0011",INSTRUCTION_SUB},
    {"Ret_0x0012",INSTRUCTION_RET},
};

FusionScript::FusionScript()
{}

int FusionScript::ReturnSyntaxCasepoint(std::string Syntax)
{
    int FoundSyntaxIndex = -1;
    for (int SyntaxIndex = 0; SyntaxIndex < InstructionSet.size(); SyntaxIndex++)
    {
        if (FoundSyntaxIndex != -1) {break;}
        if (InstructionSet[SyntaxIndex].IntermediateOpcode == Syntax)
        {FoundSyntaxIndex = InstructionSet[SyntaxIndex].Opcode;}
    }
    return FoundSyntaxIndex;
}

std::vector<std::string> FusionScript::RemoveWhitespace(const std::string& CodeString)
{
    std::istringstream          Stream(CodeString);
    std::vector<std::string>    Tokens;
    std::string                 Token;

    while (Stream >> Token)
    {Tokens.push_back(Token);}
    return Tokens;
}

void FusionScript::ParseIntermediateCode(std::string FileDirectory)
{
    std::ifstream FileSystem(FileDirectory);
    if (FileSystem.is_open())
    {
        std::string Line;
        while (std::getline(FileSystem,Line))
        {
            std::vector<std::string> BrokenLine = this->RemoveWhitespace(Line);
            this->ParsedCode.push_back(BrokenLine);
        }
    }
    FileSystem.close();

    for (int Index = 0; Index < this->ParsedCode.size(); Index++)
    {
        for (int NewIndex = 0; NewIndex < this->ParsedCode[Index].size(); NewIndex++)
        {
            switch (this->ReturnSyntaxCasepoint(this->ParsedCode[Index][NewIndex]))
            {
            case INSTRUCTION_PUSH:
                this->Stack.push_back(std::stoi(this->ParsedCode[Index][NewIndex+1]));
                break;
            case INSTRUCTION_POP:
                this->Stack.pop_back();
                break;
            case INSTRUCTION_DEFINE:
                break;
            case INSTRUCTION_UNDEFINE:
                break;
            case INSTRUCTION_GOTO:
                break;
            case INSTRUCTION_NORET:
                break;
            case INSTRUCTION_PGOTO:
                break;
            case INSTRUCTION_ADD:
                break;
            case INSTRUCTION_MUL:
                break;
            case INSTRUCTION_DIV:
                break;
            case INSTRUCTION_SUB:
                break;
            case INSTRUCTION_RET:
                break;
            default:
                break;
            }
        }
    }
}
