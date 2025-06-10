#ifndef __PARSER_HEADER__
#define __PARSER_HEADER__

#include <map>
#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include "Console.h"
#include "../Dependancies/jsonimport/json.h"

typedef enum
{
    DT_IMAGE,
    DT_MODEL,
    DT_SHADER_FRAGMENT,
    DT_SHADER_VERTEX,
    DT_CANVAS,
    DT_VERTEX,
    DT_INDICES
} DataType;

typedef struct
{
    std::string     LabelName;
    std::string     DataBytes;
    std::string     DataContents;
    DataType        TypeContent;
} ParserMap;

class Parser
{
    public:
        Parser();
        void                        FetchFromDatapack(std::string FilePath);
        std::string                 GetGlobalDatapackData(std::string FileName);
        unsigned char*              GetImageBuffer(std::string ImagePath);
        unsigned char*              GetGlobalImageBuffer(std::string ImageName);
        unsigned int                QueryDataTypeContent(std::string FileName);
        std::vector<unsigned char>  GetURIData(std::string ModelDataContents);
    private:
        nlohmann::json              Json;
        std::string                 GetDatapackData(std::string FileName);
        std::vector<ParserMap>      ParserGroup;
};

#endif