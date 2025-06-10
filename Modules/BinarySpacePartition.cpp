#include "BinarySpacePartition.h"
#include "../Common/Console.h"
#include "../Rendering/Texture.h"

BinarySpacePartition::BinarySpacePartition()
{
    this->NumberOfVerts             = 0;
    this->NumberOfFaces             = 0;
    this->NumberOfIndices           = 0;
    this->NumberOfTextures          = 0;
    this->NumberOfLightmaps         = 0;
    this->NumberOfVisibleFaces      = 0;
    this->SkipIndices               = 0;

    this->Vertices                  = NULL;
    this->Indices                   = NULL;
    this->Faces                     = NULL;

    this->TranslationMatrix         = glm::mat4(1.0f);
    this->RotationMatrix            = glm::mat4(1.0f);
    this->ScaleMatrix               = glm::mat4(1.0f);
    this->TranslationMatrix         = glm::translate(this->TranslationMatrix,this->Translation);
    this->RotationMatrix            = glm::mat4_cast(this->Rotation);
    this->ScaleMatrix               = glm::scale(this->ScaleMatrix,this->Scale);
    this->ModelMatrix               = this->TranslationMatrix*this->RotationMatrix*this->ScaleMatrix;
}

int BinarySpacePartition::FindLeaf(glm::vec3 Point)
{
    int Index       = 0;
    float Distance  = 0.0f;

    while (Index >= 0)
    {
        const BSPNode& NewNode      = this->CollisionNode[Index];
        const BSPPlane& NewPlane    = this->CollisionPlane[NewNode.PlaneIndex];
        Distance                    = NewPlane.Normal.x*Point.x+NewPlane.Normal.y*Point.y+NewPlane.Normal.z*Point.z-NewPlane.Distance;
        
        if (Distance >= 0)
        {Index = NewNode.Front;}
        else
        {Index = NewNode.Back;}
    }
    return ~Index;
}

glm::vec3 BinarySpacePartition::CheckNextPosition(glm::vec3 Start,glm::vec3 End)
{
    for (float Height = 1.0f; Height < MAX_STEP_HEIGHT; Height++)
    {
        this->Collided  = false;
        this->TryStep   = false;

        glm::vec3 StepStart     = glm::vec3(Start.x,Start.y+Height,Start.z);
        glm::vec3 StepEnd       = glm::vec3(End.x,Start.y+Height,End.z);
        glm::vec3 StepPosition  = this->Trace(StepStart,StepEnd);

        if (!this->Collided)
        {
            return StepPosition;
        }
    }
    return Start;
}

glm::vec3 BinarySpacePartition::Trace(glm::vec3 Start,glm::vec3 End)
{
    this->TraceRatio = 1.0f;
    this->CheckNode(0,0.0f,1.0f,Start,End);

    if (this->TraceRatio == 1.0f) {return End;}
    else
    {
        glm::vec3 NewPosition   = Start+((End-Start)*this->TraceRatio);
        glm::vec3 Move          = End-NewPosition;
        float Distance          = glm::dot(Move,this->CollisionNormal);
        glm::vec3 EndPosition   = End-this->CollisionNormal*Distance;
        NewPosition             = this->Trace(NewPosition,EndPosition);

        if (this->CollisionNormal.y > 0.1f || this->Grounded) {this->Grounded = true;}
        else                                                  {this->Grounded = false;}
        return NewPosition;
    }
}

glm::vec3 BinarySpacePartition::TraceSphere(glm::vec3 Start,glm::vec3 End,float Radius)
{
    // Start = glm::vec3(this->ModelMatrix*glm::vec4(Start,1.0f));
    // End = glm::vec3(this->ModelMatrix*glm::vec4(End,1.0f));

    this->TraceType = TYPE_SPHERE;
    this->Collided  = false;
    this->TryStep   = false;
    this->Grounded  = false;
    this->TraceRadius = Radius;
    
    glm::vec3 NewPosition = this->Trace(Start,End);
    if (this->Collided && this->TryStep) {SysPrint("Collided!",2); NewPosition = this->CheckNextPosition(NewPosition,End);}
    return NewPosition;
}

void BinarySpacePartition::CheckNode(int NodeIndex,float StartRatio,float EndRatio,glm::vec3 Start,glm::vec3 End)
{
    if (NodeIndex < 0)
    {
        BSPLeaf *NewLeaf = &this->CollisionLeaf[-(NodeIndex)+1];
        for (int Index = 0; Index < NewLeaf->NumLeafBrush; Index++)
        {
            BSPBrush *NewBrush = &this->CollisionBrushes[this->LeafBrushes[NewLeaf->FirstLeafBrush+Index]];
            if ((NewBrush->NumberSide > 0) && (this->CollisionTextures[NewBrush->TextureIndex].Flags & 1))
            {
                this->CheckBrush(NewBrush,Start,End);
            }
        }
        return;
    }

    BSPNode *NewNode        = &this->CollisionNode[NodeIndex];
    BSPPlane *NewPlane      = &this->CollisionPlane[NewNode->PlaneIndex];
    float StartDistance     = glm::dot(Start,NewPlane->Normal)-NewPlane->Distance;
    float EndDistance       = glm::dot(End,NewPlane->Normal)-NewPlane->Distance;
    float Offset            = 0.0f;

    if (this->TraceType == TYPE_SPHERE)     {Offset = this->TraceRadius;}
    else if (this->TraceType == TYPE_BOX)   {Offset = (float)(fabs(this->Extents.x*NewPlane->Normal.x)+fabs(this->Extents.y*NewPlane->Normal.y)+fabs(this->Extents.z*NewPlane->Normal.z));}

    if (StartDistance >= Offset && EndDistance >= Offset)
    {this->CheckNode(NewNode->Front,StartDistance,EndDistance,Start,End);}
    else if (StartDistance < -Offset && EndDistance < -Offset)
    {this->CheckNode(NewNode->Back,StartDistance,EndDistance,Start,End);}
    else
    {
        float Ratio         = 1.0f;
        float Ratio2        = 0.0f;
        float MiddleRatio   = 0.0f;
        glm::vec3 Middle;
        int Side            = NewNode->Front;

        if (StartDistance < EndDistance)
        {
            Side = NewNode->Back;
            float InverseDistance = 1.0f/(StartDistance-EndDistance);
            Ratio   = (StartDistance-Offset-KEPSILON)*InverseDistance;
            Ratio2  = (StartDistance+Offset+KEPSILON)*InverseDistance;
        }
        else if (StartDistance > EndDistance)
        {
            float InverseDistance = 1.0f/(StartDistance-EndDistance);
            Ratio   = (StartDistance+Offset+KEPSILON)*InverseDistance;
            Ratio2  = (StartDistance-Offset-KEPSILON)*InverseDistance;
        }

        if (Ratio < 0.0f)       {Ratio = 0.0f;}
        else if (Ratio > 1.0f)  {Ratio = 1.0f;}
        if (Ratio2 < 0.0f)      {Ratio2 = 0.0f;}
        else if (Ratio2 > 1.0f) {Ratio2 = 1.0f;}

        MiddleRatio = StartRatio+((EndRatio-StartRatio)*Ratio);
        Middle      = Start+((End-Start)*Ratio);
        this->CheckNode(Side,StartRatio,MiddleRatio,Start,Middle);

        MiddleRatio = StartRatio+((EndRatio-StartRatio)*Ratio2);
        Middle      = Start+((End-Start)*Ratio2);

        if (Side == NewNode->Back)
        {this->CheckNode(NewNode->Front,MiddleRatio,EndRatio,Middle,End);}
        else
        {this->CheckNode(NewNode->Back,MiddleRatio,EndRatio,Middle,End);}
    }
}

void BinarySpacePartition::CheckBrush(BSPBrush *NewBrush,glm::vec3 Start,glm::vec3 End)
{
    float StartRatio    = -1.0f;
    float EndRatio      = 1.0f;
    bool StartOut       = false;

    for (int Index = 0; Index < NewBrush->NumberSide; Index++)
    {
        BSPBrushSide *NewBrushSide  = &this->CollisionBrushSides[NewBrush->FirstSide+Index];
        BSPPlane *NewPlane          = &this->CollisionPlane[NewBrushSide->PlaneIndex];

        float Offset = 0.0f;
        if (this->TraceType == TYPE_SPHERE) {Offset = this->TraceRadius;}

        float StartDistance = glm::dot(Start,NewPlane->Normal)-(NewPlane->Distance+Offset);
        float EndDistance   = glm::dot(End,NewPlane->Normal)-(NewPlane->Distance+Offset);

        glm::vec3 VectorOffset = glm::vec3(0.0f,0.0f,0.0f);
        if (this->TraceType == TYPE_BOX)    
        {
            VectorOffset.x = ((NewPlane->Normal.x < 0) ? this->TraceMax.x : this->TraceMins.x);
            VectorOffset.y = ((NewPlane->Normal.y < 0) ? this->TraceMax.y : this->TraceMins.y);
            VectorOffset.z = ((NewPlane->Normal.z < 0) ? this->TraceMax.z : this->TraceMins.z);

            StartDistance   = glm::dot(Start+VectorOffset,NewPlane->Normal)-NewPlane->Distance;
            EndDistance     = glm::dot(End+VectorOffset,NewPlane->Normal)-NewPlane->Distance;
        }
        if (StartDistance > 0)                      {StartOut = true;}
        if (StartDistance > 0 && EndDistance > 0)   {return;}
        if (StartDistance <= 0 && EndDistance <= 0) {continue;}

        if (StartDistance > EndDistance)
        {
            float Ratio1 = (StartDistance-KEPSILON)/(StartDistance-EndDistance);
            if (Ratio1>StartRatio)
            {
                StartRatio              = Ratio1;
                this->Collided          = true;
                this->CollisionNormal   = NewPlane->Normal;

                if ((Start.x != End.x || Start.z != End.z) && NewPlane->Normal.y != 1)  {this->TryStep = true;}
                if (this->CollisionNormal.y > 0.2f)                                     {this->Grounded = true;}
            }
        }
        else
        {
            float Ratio = (StartDistance+KEPSILON)/(StartDistance-EndDistance);
            if (Ratio < EndRatio) {EndRatio = Ratio;}
        }
    }
    
    if (StartOut == false) {return;}
    if (StartRatio < EndRatio)
    {
        if (StartRatio > -1 && StartRatio < this->TraceRatio)
        {
            if (StartRatio < 0) {StartRatio = 0;}
            this->TraceRatio = StartRatio;
        }
    }
}

glm::vec3 BinarySpacePartition::SpawnPlayerInBSP()
{return this->PlayerSpawnPositions[rand()%this->PlayerSpawnPositions.size()];}

void BinarySpacePartition::SetupBinarySpacePartition(Parser* EngineParser)
{
    this->BuildVertexBuffer();
    this->GenerateTextures(EngineParser);
    this->GenerateLightMaps();
}

void BinarySpacePartition::GetCollisionData()
{
    this->NumberOfPlanes    = this->Lumps[KPlanes].Length/sizeof(BSPPlane);
    this->CollisionPlane    = new BSPPlane[this->NumberOfPlanes];
    fseek(this->FilePointer,this->Lumps[KPlanes].Offset,SEEK_SET);
    fread(this->CollisionPlane,sizeof(BSPPlane),this->NumberOfPlanes,this->FilePointer);

    this->NumberOfNodes     = this->Lumps[KNodes].Length/sizeof(BSPNode);
    this->CollisionNode     = new BSPNode[this->NumberOfNodes];
    fseek(this->FilePointer,this->Lumps[KNodes].Offset,SEEK_SET);
    fread(this->CollisionNode,sizeof(BSPNode),this->NumberOfNodes,this->FilePointer);

    this->NumberOfLeafs     = this->Lumps[KLeafs].Length/sizeof(BSPLeaf);
    this->CollisionLeaf     = new BSPLeaf[this->NumberOfLeafs];
    fseek(this->FilePointer,this->Lumps[KLeafs].Offset,SEEK_SET);
    fread(this->CollisionLeaf,sizeof(BSPLeaf),this->NumberOfLeafs,this->FilePointer);

    this->NumberOfBrushes   = this->Lumps[KBrushes].Length/sizeof(BSPBrush);
    this->CollisionBrushes  = new BSPBrush[this->NumberOfBrushes];
    fseek(this->FilePointer,this->Lumps[KBrushes].Offset,SEEK_SET);
    fread(this->CollisionBrushes,sizeof(BSPBrush),this->NumberOfBrushes,this->FilePointer);

    this->NumberOfBrushSides    = this->Lumps[KBrushSides].Length/sizeof(BSPBrushSide);
    this->CollisionBrushSides   = new BSPBrushSide[this->NumberOfBrushSides];
    fseek(this->FilePointer,this->Lumps[KBrushSides].Offset,SEEK_SET);
    fread(this->CollisionBrushSides,sizeof(BSPBrushSide),this->NumberOfBrushSides,this->FilePointer);

    this->NumberOfTexturesCollision         = this->Lumps[KTextures].Length/sizeof(BSPTexture);
    this->CollisionTextures                 = new BSPTexture[this->NumberOfTexturesCollision];
    fseek(this->FilePointer,this->Lumps[KTextures].Offset,SEEK_SET);
    fread(this->CollisionTextures,sizeof(BSPTexture),this->NumberOfTexturesCollision,this->FilePointer);

    this->NumberOfLeafBrushes   = this->Lumps[KLeafBrushes].Length/sizeof(int);
    this->LeafBrushes           = new int[this->NumberOfLeafBrushes];
    fseek(this->FilePointer,this->Lumps[KLeafBrushes].Offset,SEEK_SET);
    fread(this->LeafBrushes,sizeof(int),this->NumberOfLeafBrushes,this->FilePointer);
}

void BinarySpacePartition::GetPlayerSpawnPositions()
{
    if (this->FilePointer == NULL) {SysPrint("Failed to get position: File has been closed.",2);}
    int EntityLength = this->Lumps[KEntites].Length;
    int EntityOffset = this->Lumps[KEntites].Offset;
    size_t Position  = 0;

    char* EntityData = new char[EntityLength+1];
    fseek(this->FilePointer,EntityOffset,SEEK_SET);
    fread(EntityData,1,EntityLength,this->FilePointer);
    EntityData[EntityLength] = '\0';

    std::string EntityText(EntityData);
    delete[] EntityData;

    while ((Position = EntityText.find("{",Position)) != std::string::npos)
    {
        size_t EndPosition = EntityText.find("}",Position);
        if (EndPosition == std::string::npos) {break;}

        std::string EntityBlock = EntityText.substr(Position,EndPosition-Position);
        if (EntityBlock.find("\"classname\" \"info_player_deathmatch\"") != std::string::npos || EntityBlock.find("\"classname\" \"info_player_start\"") != std::string::npos)
        {
            size_t OriginKey = EntityBlock.find("\"origin\"");
            if (OriginKey != std::string::npos)
            {
                size_t FirstValueQuote = EntityBlock.find("\"",OriginKey+8);
                if (FirstValueQuote != std::string::npos)
                {
                    size_t SecondValueQuote = EntityBlock.find("\"",FirstValueQuote+1);
                    if (SecondValueQuote != std::string::npos)
                    {
                        std::string NewOriginString = EntityBlock.substr(FirstValueQuote+1,SecondValueQuote-FirstValueQuote-1);
                        float X,Y,Z;
                        if (sscanf(NewOriginString.c_str(),"%f %f %f",&X,&Y,&Z) == 3)
                        {
                            float Temporary = Y;
                            Y = Z;
                            Z = -Temporary;
                            this->PlayerSpawnPositions.push_back(glm::vec3(this->ModelMatrix*glm::vec4(X,Y,Z,1.0f)));
                        }
                        else {SysPrint("Failed to parse origin on: "+NewOriginString, 2);}
                    }
                }
            }
        }
        Position = EndPosition + 1;
    }
}

void BinarySpacePartition::LoadBinarySpacePartition(std::string FileName)
{
    SysPrint("Using "+FileName,1);
    if ((this->FilePointer = fopen(FileName.c_str(),"rb")) == NULL)
    {SysPrint("Cannot open BSP File",2); return;}

    fread(&this->Header,1,sizeof(BSPHeader),this->FilePointer);
    fread(&this->Lumps,KMaxLumps,sizeof(BSPLump),this->FilePointer);

    if (this->Header.Version != 0x2e)
    {SysPrint("Failed to load bsp. Not an IBSP.",2);}

    this->NumberOfVerts     = this->Lumps[KVertices].Length/sizeof(BSPVertex);
    this->Vertices          = new BSPVertex[this->NumberOfVerts];

    this->NumberOfFaces     = this->Lumps[KFaces].Length/sizeof(BSPFace);
    this->Faces             = new BSPFace[this->NumberOfFaces];

    this->NumberOfIndices   = this->Lumps[KIndices].Length/sizeof(int);
    this->Indices           = new int[this->NumberOfIndices];

    this->NumberOfTextures  = this->Lumps[KTextures].Length/sizeof(BSPTexture);
    fseek(this->FilePointer,this->Lumps[KVertices].Offset,SEEK_SET);

    for (int Index = 0; Index < this->NumberOfVerts; Index++)
    {
        fread(&this->Vertices[Index],1,sizeof(BSPVertex),this->FilePointer);
        float Temporary                     = this->Vertices[Index].Position.y;
        this->Vertices[Index].Position.y    = this->Vertices[Index].Position.z;
        this->Vertices[Index].Position.z    = -Temporary; 
    }

    this->GetCollisionData();
    this->GetPlayerSpawnPositions();
    BSPTexture* Textures = new BSPTexture[this->NumberOfTextures];
    fseek(this->FilePointer,this->Lumps[KIndices].Offset,SEEK_SET);
    fread(this->Indices,this->NumberOfIndices,sizeof(int),this->FilePointer);
    fseek(this->FilePointer,this->Lumps[KFaces].Offset,SEEK_SET);
    fread(this->Faces,this->NumberOfFaces,sizeof(BSPFace),this->FilePointer);
    fseek(this->FilePointer,this->Lumps[KTextures].Offset,SEEK_SET);
    fread(Textures,this->NumberOfTextures,sizeof(BSPTexture),this->FilePointer);

    for (int Index = 0; Index < this->NumberOfTextures; Index++)
    {
        strcpy(this->TNameCache[Index],Textures[Index].StringName);
        strcat(this->TNameCache[Index],".tga");
        std::filesystem::path OldFileName   = this->TNameCache[Index];
        std::string NewFileName             = OldFileName.filename().string();
        this->TName.push_back(NewFileName);
    }

    this->NumberOfLightmaps = this->Lumps[KLightmaps].Length/sizeof(BSPLightmap);
    BSPLightmap* LightMaps  = new BSPLightmap[this->NumberOfLightmaps];
    fseek(this->FilePointer,this->Lumps[KLightmaps].Offset,SEEK_SET);

    for (int Index = 0; Index < this->NumberOfLightmaps; Index++)
    {
        fread(&LightMaps[Index],1,sizeof(BSPLightmap),this->FilePointer);
        this->RendBuffer.LightMaps.push_back(LightMaps[Index]);
    }

    delete[] Textures;
    delete[] LightMaps;

    fclose(this->FilePointer);
}

void BinarySpacePartition::BuildVertexBuffer()
{
    for (int Index = 0; Index < this->NumberOfFaces; Index++)
    {
        BSPFace* NewFace = &this->Faces[Index];
        if (NewFace->Type == FACE_POLYGON) {this->SkipIndices += NewFace->NumberOfIndices;}

        this->CreateVertexBuffer(Index);
        this->CreateIndices(Index);
        this->CreateRenderBuffers(Index);
        this->NumberOfVisibleFaces++;
    }
}

void BinarySpacePartition::CreateVertexBuffer(int Index)
{
    BSPFace* NewFace = &this->Faces[Index];
    
    for (int VertexIndex = 0; VertexIndex < NewFace->NumberOfVertex; VertexIndex++)
    {
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].Position.x);
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].Position.y);
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].Position.z);
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].TextureCoord.x);
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].TextureCoord.y);
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].LightMapCoord.x);
        this->RendBuffer.FaceVertexBuffers[Index].push_back(this->Vertices[NewFace->StartVertexIndex+VertexIndex].LightMapCoord.y);
    }
}

void BinarySpacePartition::CreateIndices(int Index)
{
    BSPFace* NewFace    = &this->Faces[Index];
    int IndexOffset     = NewFace->StartIndex;
    for (int IndicesIndex = 0; IndicesIndex < NewFace->NumberOfIndices; IndicesIndex++)
    {this->RendBuffer.FaceVertexIndex[Index].push_back(this->Indices[IndexOffset+IndicesIndex]);}
}

void BinarySpacePartition::Debug(int Index)
{
    SysPrint("Face:---->"+std::to_string(Index),2);
    for (unsigned int NewIndex = 0; NewIndex < this->RendBuffer.FaceVertexBuffers[Index].size(); NewIndex++)
    {SysPrint(">-"+std::to_string(this->RendBuffer.FaceVertexBuffers[Index][NewIndex]),2);}

    for (unsigned int NewIndex = 0; NewIndex < this->RendBuffer.FaceVertexBuffers[Index].size(); NewIndex++)
    {SysPrint(">-"+std::to_string(this->RendBuffer.FaceVertexIndex[Index][NewIndex]),2);}

    SysPrint("Vbuffer"+std::to_string(sizeof(GLfloat)*this->RendBuffer.FaceVertexBuffers[Index].size()),2);
    SysPrint("Ibuffer"+std::to_string(sizeof(GLuint)*this->RendBuffer.FaceVertexIndex[Index].size()),2);
    SysPrint("Endface",2);
}

void BinarySpacePartition::CreateRenderBuffers(int Index)
{
    glGenVertexArrays(1,&(this->FaceBufferArray.FaceBufferIndex[Index].VertexArray));
    glBindVertexArray((this->FaceBufferArray.FaceBufferIndex[Index].VertexArray));

    glGenBuffers(1,&(this->FaceBufferArray.FaceBufferIndex[Index].VertexBuffers));
    glBindBuffer(GL_ARRAY_BUFFER,this->FaceBufferArray.FaceBufferIndex[Index].VertexBuffers);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*this->RendBuffer.FaceVertexBuffers[Index].size(),&this->RendBuffer.FaceVertexBuffers[Index].front(),GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,7*sizeof(GLfloat),(GLvoid*)(5*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}

void BinarySpacePartition::GenerateTextures(Parser* EngineParser)
{
    GLuint TextureIDs;
    GLfloat Anisope = 8.0f;
    int Width,Height;
    int NumberOfChannels    = 3;
    GLuint MissingIds       = 1;
    
    glGenTextures(1,&MissingIds);
    unsigned char* Image = EngineParser->GetGlobalImageBuffer("missing.imgbuf");

    glBindTexture(GL_TEXTURE_2D,MissingIds);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1024,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,Image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(Image);

    for (int Index = 0; Index < this->NumberOfFaces; Index++)
    {
        BSPFace* NewFace                            = &this->Faces[Index];
        this->RendBuffer.TextureOptimised[Index]    = NewFace->TextureID+2;
    }

    std::vector<GLuint> MissingTextureIds;
    for (int Index = 0; Index < this->NumberOfTextures; Index++)
    {
        unsigned char* Image = stbi_load(("../../Fusion/tex/"+TName[Index]).c_str(),&Width,&Height,&NumberOfChannels,3);
        if (Image)
        {
            glGenTextures(1,&TextureIDs);
            glBindTexture(GL_TEXTURE_2D,TextureIDs);

            glGetFloatv(0x84FF,&Anisope);
            glTexParameterf(GL_TEXTURE_2D,0x84FF,Anisope);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);

            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,Width,Height,0,GL_RGB,GL_UNSIGNED_BYTE,Image);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(Image);
        }
        else
        {
            SysPrint("Failed to load "+this->TName[Index],2);
            stbi_image_free(Image);
            glGenTextures(1,&TextureIDs);
            MissingTextureIds.push_back(TextureIDs);
            glBindTexture(GL_TEXTURE_2D,MissingIds);
        }
    }

    for (GLuint Index = 0; Index < this->RendBuffer.TextureOptimised.size(); Index++)
    {
        for (auto Value: MissingTextureIds)
        {
            if (this->RendBuffer.TextureOptimised[Index] == Value)
            {this->RendBuffer.TextureOptimised[Index] = MissingIds;}
        }
    }
}

void BinarySpacePartition::GenerateLightMaps()
{
    GLfloat WhiteLightMap[] ={
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glGenTextures(1,&MissingLightmapIDs);
    glBindTexture(GL_TEXTURE_2D,MissingLightmapIDs);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,2,2,0,GL_RGB,GL_FLOAT,&WhiteLightMap);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    this->LightmapTexturesID = new GLuint[this->NumberOfLightmaps];
    glGenTextures(this->RendBuffer.LightMaps.size(),this->LightmapTexturesID);

    for (GLuint Index = 0; Index < this->RendBuffer.LightMaps.size(); Index++)
    {
        glBindTexture(GL_TEXTURE_2D,this->LightmapTexturesID[Index]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,128,128,0,GL_RGB,GL_UNSIGNED_BYTE,&this->RendBuffer.LightMaps[Index].ImageBits);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void BinarySpacePartition::RenderOneFace(int Index,unsigned int Program,glm::mat4 ViewMatrix,glm::mat4 ProjectionMatrix)
{
    glBindVertexArray(this->FaceBufferArray.FaceBufferIndex[Index].VertexArray);
    BSPFace* NewFace  = &this->Faces[Index];
    GLuint TextureID  = this->RendBuffer.TextureOptimised[Index];
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,TextureID);

    glActiveTexture(GL_TEXTURE1);
    if (NewFace->LightMapID >= 0)   {glBindTexture(GL_TEXTURE_2D,this->LightmapTexturesID[NewFace->LightMapID]);}
    else                            {glBindTexture(GL_TEXTURE_2D,this->MissingLightmapIDs);}

    glUniformMatrix4fv(glGetUniformLocation(Program,"model"),1,GL_FALSE,glm::value_ptr(this->ModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(Program,"projection"),1,GL_FALSE,glm::value_ptr(glm::mat4(ProjectionMatrix)));
    glUniformMatrix4fv(glGetUniformLocation(Program,"view"),1,GL_FALSE,glm::value_ptr(glm::mat4(ViewMatrix)));
    glUniform1i(glGetUniformLocation(Program,"s_bspTexture"),0);
    glUniform1i(glGetUniformLocation(Program,"s_bspLightmap"),1);
    glUseProgram(Program);
    glDrawElements(GL_TRIANGLES,NewFace->NumberOfIndices,GL_UNSIGNED_INT,&this->Indices[NewFace->StartIndex]);
    glBindVertexArray(0);
}

void BinarySpacePartition::RenderAllFaces(unsigned int Program,glm::mat4 ViewMatrix,glm::mat4 ProjectionMatrix)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    for (auto& Face: this->RendBuffer.FaceVertexBuffers) {this->RenderOneFace(Face.first,Program,ViewMatrix,ProjectionMatrix);}
}

void BinarySpacePartition::DestroyScene()
{
    for (int Index = 0; Index < this->NumberOfFaces; Index++) {glDeleteVertexArrays(1,&(this->FaceBufferArray.FaceBufferIndex[Index].VertexArray));}
    for (int Index = 0; Index < this->NumberOfFaces; Index++) {glDeleteBuffers(1,&(this->FaceBufferArray.FaceBufferIndex[Index].VertexBuffers));}
}