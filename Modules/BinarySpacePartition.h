#ifndef __BINARY_SPACE_PARTITIONING_HEADER__
#define __BINARY_SPACE_PARTITIONING_HEADER__

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define MAX_TEXTURES 1000
#define FACE_POLYGON 1

#define MAX_STEP_HEIGHT 22.0f
#define TYPE_RAY 0
#define TYPE_SPHERE 1
#define TYPE_BOX 2

#define KEPSILON 0.03125f

#include <cstring>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <filesystem>
#include "../../Dependancies/glad/glad.h"
#include "../../Dependancies/glm/glm.hpp"
#include "../../Dependancies/glm/gtc/matrix_transform.hpp"
#include "../../Dependancies/glm/gtx/rotate_vector.hpp"
#include "../../Dependancies/glm/gtx/vector_angle.hpp"
#include "../Common/Parser.h"
typedef unsigned char Byte;

struct BSPPlane
{
    glm::vec3   Normal;
    float       Distance;
};

struct BSPNode
{
    int PlaneIndex;
    int Front;
    int Back;
    int MinBounds[3];
    int MaxBounds[3];
};

struct BSPLeaf
{
    int Cluster;
    int Area;
    int MinBounds[3];
    int MaxBounds[3];
    int FirstLeafFace;
    int NumLeafFace;
    int FirstLeafBrush;
    int NumLeafBrush;
};

struct BSPBrush
{
    int FirstSide;
    int NumberSide;
    int TextureIndex;
};

struct BSPBrushSide
{
    int PlaneIndex;
    int TextureIndex;
};

struct BSPHeader
{
    char    StringID[4];
    int     Version;
};

struct BSPLump
{
    int     Offset;
    int     Length;
};

struct BSPVertex
{
    glm::vec3   Position;
    glm::vec2   TextureCoord;
    glm::vec2   LightMapCoord;
    glm::vec3   Normal;
    Byte        Colour[4];
};

struct BSPFace
{
    int         TextureID;
    int         Effect;
    int         Type;
    int         StartVertexIndex;
    int         NumberOfVertex;
    int         StartIndex;
    int         NumberOfIndices;
    int         LightMapID;
    int         LightMapCorner[2];
    int         MapSize[2];
    glm::vec3   MapPosition;
    glm::vec3   MapVectors[2];
    glm::vec3   Normal;
    int         Size[2];
};

struct BSPTexture
{
    char        StringName[64];
    int         Flags;
    int         Contents;
};

struct BSPLightmap
{
    Byte        ImageBits[128][128][3];
};

struct FaceBuffers
{
    unsigned int VertexBuffers;
    unsigned int VertexArray;
    unsigned int ElementBuffers;
};

struct FaceBufferArray
{
    std::map<int,FaceBuffers> FaceBufferIndex;
};

struct RenderBuffers
{
    std::map<int,std::vector<GLfloat>>          FaceVertexBuffers;
    std::map<int,std::vector<GLuint>>           FaceVertexIndex;
    std::map<int,std::string>                   TextureVector;
    std::vector<BSPLightmap>                    LightMaps;
    std::map<GLuint,GLuint>                     TextureOptimised;
    std::map<GLuint,GLuint>                     LightMapOptimised;
};

struct TraceResult
{
    bool        AllSolid;
    bool        StartSolid;
    bool        Hit;
    float       Fraction;
    glm::vec3   HitPoint;
    glm::vec3   HitNormal;
};

enum Lumps
{
    KEntites = 0,
    KTextures,
    KPlanes,
    KNodes,
    KLeafs,
    KLeafFaces,
    KLeafBrushes,
    KModels,
    KBrushes,
    KBrushSides,
    KVertices,
    KIndices,
    KShaders,
    KFaces,
    KLightmaps,
    KLightVolumes,
    KVisData,
    KMaxLumps
};

class BinarySpacePartition
{
    public:
        BinarySpacePartition();
        void LoadBinarySpacePartition(std::string FileName);
        void SetupBinarySpacePartition(Parser* EngineParser);
        void RenderAllFaces(unsigned int Program,glm::mat4 ViewMatrix,glm::mat4 ProjectionMatrix);
        void DestroyScene();
        void CheckNode(int NodeIndex,float StartRatio,float EndRatio,glm::vec3 Start,glm::vec3 End);
        void CheckBrush(BSPBrush *NewBrush,glm::vec3 Start,glm::vec3 End);
        int FindLeaf(glm::vec3 Point);

        glm::vec3 CheckNextPosition(glm::vec3 Start,glm::vec3 End);
        glm::vec3 Trace(glm::vec3 Start,glm::vec3 End);
        glm::vec3 TraceSphere(glm::vec3 Start,glm::vec3 End,float Radius);
        glm::vec3 SpawnPlayerInBSP();
    private:
        glm::vec3   Extents;
        glm::vec3   TraceMins;
        glm::vec3   TraceMax;
        glm::vec3   CollisionNormal;
        bool        Collided;
        bool        Grounded;
        bool        TryStep;
        float       TraceRatio;
        float       TraceRadius;
        int         TraceType;

        void RenderOneFace(int Index,unsigned int Program,glm::mat4 ViewMatrix,glm::mat4 ProjectionMatrix);
        void GenerateTextures(Parser* EngineParser);
        void GenerateLightMaps();
        void GetPlayerSpawnPositions();
        void BuildVertexBuffer();
        void GetCollisionData();
        void CreateVertexBuffer(int Index);
        void CreateIndices(int Index);
        void CreateRenderBuffers(int Index);
        void Debug(int Index);

        FILE*   FilePointer = NULL;
        int     NumberOfVerts;
        int     NumberOfFaces;
        int     NumberOfIndices;
        int     NumberOfTextures;
        int     NumberOfLightmaps;
        int     NumberOfVisibleFaces;
        int     SkipIndices;
        int     BinaryRecursion = 0;
        char    TNameCache[MAX_TEXTURES][64];
        std::vector<std::string>    TName;

        BSPHeader   Header              = {0};
        BSPLump     Lumps[KMaxLumps]    = {0};

        BSPPlane*       CollisionPlane;
        BSPNode*        CollisionNode;
        BSPLeaf*        CollisionLeaf;
        BSPBrush*       CollisionBrushes;
        BSPBrushSide*   CollisionBrushSides;
        BSPTexture*     CollisionTextures;

        int* LeafBrushes;
        int NumberOfTexturesCollision;
        int NumberOfPlanes;
        int NumberOfNodes;
        int NumberOfLeafs;
        int NumberOfBrushes;
        int NumberOfBrushSides;
        int NumberOfLeafBrushes;

        GLuint* BSPTextures;
        GLuint* LightmapTexturesID;
        GLuint  MissingLightmapIDs;
        GLuint  MissingIDs;
        glm::mat4   TranslationMatrix;
        glm::mat4   RotationMatrix;
        glm::mat4   ScaleMatrix;
        glm::mat4   ModelMatrix;
        std::vector<glm::vec3>  PlayerSpawnPositions;

        glm::vec3 Translation   = glm::vec3(0.0f,0.0f,0.0f);
        glm::quat Rotation      = glm::quat(1.0f,0.0f,0.0f,0.0f);
        glm::vec3 Scale         = glm::vec3(0.25f,0.25f,0.25f);

        RenderBuffers       RendBuffer;
        FaceBufferArray     FaceBufferArray;
        BSPVertex*          Vertices;
        BSPFace*            Faces;
        int*                Indices;
};

#endif