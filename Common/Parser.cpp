#include "Parser.h"
#include "Shader.h"
#include "../Enviroments/Rendering/Texture.h"
#include "../Dependancies/zip/zip_file.h"

miniz_cpp::zip_file *Datapack;

Parser::Parser() {}

void Parser::FetchFromDatapack(std::string FilePath)
{
    miniz_cpp::zip_file ImportDatapack(FilePath.c_str());
    Datapack                            = &ImportDatapack;
    std::vector<std::string> NewFile    = Datapack->namelist();
    SysPrint("[----------Loading Assets----------]",1);
    for (int Index = 0; Index < NewFile.size(); Index++)
    {
        SysPrint("Loading "+NewFile[Index],1);
        if (NewFile[Index].find(".imgbuf") != std::string::npos)
        {
            ParserMap NewMap;
            std::string BufferContent       = this->GetDatapackData(NewFile[Index]);
            NewMap.DataBytes                = BufferContent;
            NewMap.TypeContent              = DT_IMAGE;
            NewMap.LabelName                = NewFile[Index];
            this->ParserGroup.push_back(NewMap);
        }
        else if (NewFile[Index].find(".ttf") != std::string::npos)
        {
            ParserMap NewMap;
            std::string BufferContents      = this->GetDatapackData(NewFile[Index]);
            // std::cout << BufferContents << "\n";
        }
        else if (NewFile[Index].find(".gltf") != std::string::npos || NewFile[Index].find(".canv") != std::string::npos || NewFile[Index].find(".bin") != std::string::npos)
        {
            ParserMap NewMap;
            std::string PackData            = this->GetDatapackData(NewFile[Index]);
            NewMap.DataContents             = PackData;
            NewMap.LabelName                = NewFile[Index];
            if (NewFile[Index].find(".canv") != std::string::npos)  {NewMap.TypeContent = DT_CANVAS;}
            else                                                    {NewMap.TypeContent = DT_MODEL;}
            this->ParserGroup.push_back(NewMap);
        }
        else if (NewFile[Index].find(".vt") != std::string::npos || NewFile[Index].find(".ind") != std::string::npos)
        {
            ParserMap NewMap;
            std::string PackData            = this->GetDatapackData(NewFile[Index]);
            NewMap.DataContents             = PackData;
            NewMap.LabelName                = NewFile[Index];
            if (NewFile[Index].find(".vt") != std::string::npos) {NewMap.TypeContent = DT_VERTEX;}
            else                                                 {NewMap.TypeContent = DT_INDICES;}
            this->ParserGroup.push_back(NewMap);
        }
        else if (NewFile[Index].find(".vert") != std::string::npos || NewFile[Index].find(".frag") != std::string::npos)
        {
            ParserMap NewMap;
            std::string PackData            = this->GetDatapackData(NewFile[Index]);
            NewMap.DataContents             = PackData;
            NewMap.LabelName                = NewFile[Index];
            if (NewFile[Index].find(".vert") != std::string::npos)  {NewMap.TypeContent = DT_SHADER_VERTEX;}
            else                                                    {NewMap.TypeContent = DT_SHADER_FRAGMENT;}
            this->ParserGroup.push_back(NewMap);
        }
        else 
        {SysPrint(NewFile[Index]+" isnt a supported file.",2);}
    }
    SysPrint("[-----"+FilePath+" Datapack Loaded-----]",1);
}

std::string Parser::GetDatapackData(std::string FileName)
{
    std::stringstream Stream;
    Stream << Datapack->open(FileName).rdbuf();
    return Stream.str();
}

std::vector<unsigned char> Parser::GetURIData(std::string ModelDataContents)
{
    std::stringstream Stream;
    this->Json              = nlohmann::json::parse(ModelDataContents);
    std::string BufferURI   = this->Json["buffers"][0]["uri"];
    std::string Data = this->GetGlobalDatapackData(BufferURI);
    std::vector<unsigned char> NewData(Data.begin(),Data.end());
    return NewData;
}

std::string Parser::GetGlobalDatapackData(std::string FileName)
{
    for (int Index = 0; Index < this->ParserGroup.size(); Index++)
    {
        if (this->ParserGroup[Index].LabelName == FileName)
        {return this->ParserGroup[Index].DataContents;}
    }
    SysPrint("Failed to get Datapack Data for "+FileName,2);
    return "";
}

unsigned int Parser::QueryDataTypeContent(std::string FileName)
{
    int FileIndex = -1;
    for (int Index = 0; Index < this->ParserGroup.size(); Index++)
    {if (this->ParserGroup[Index].LabelName == FileName) {FileIndex = Index;}}
    
    if (FileIndex != -1)    {return this->ParserGroup[FileIndex].TypeContent;}
    else                    {SysPrint("Failed to query file: "+FileName,2);}
    return FileIndex;
}

unsigned char* Parser::GetGlobalImageBuffer(std::string ImageName)
{
    if (ImageName.find(".imgbuf") != std::string::npos) 
    {
        for (int Index = 0; Index < this->ParserGroup.size(); Index++)
        {
            if (this->ParserGroup[Index].LabelName == ImageName)
            {
                unsigned char* Pointer = new unsigned char[this->ParserGroup[Index].DataBytes.size()+1];
                std::memcpy(Pointer,this->ParserGroup[Index].DataBytes.c_str(),this->ParserGroup[Index].DataBytes.size()+1);
                return Pointer;
            }
        }
    }
    SysPrint("Failed to get ImageBuffer for "+ImageName,2);
    return (unsigned char*)("");
}

unsigned char* Parser::GetImageBuffer(std::string ImageName)
{
    if (!Datapack->has_file(ImageName.c_str())) {SysPrint(ImageName+" is not found within the Datapack.",2);}
    std::stringstream   Stream;
    Stream << Datapack->open(ImageName.c_str()).rdbuf();
    std::string NewString = Stream.str();
    std::basic_string<unsigned char> Unsigned(NewString.begin(),NewString.end());
    return &Unsigned[0];
}