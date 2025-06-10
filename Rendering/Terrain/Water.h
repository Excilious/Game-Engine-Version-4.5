#ifndef __WATER_HEADER__
#define __WATER_HEADER__

#define WATER_GRID_SIZE 1024
#define WATER_GRID_SPACING 0.1f

#include <vector>
#include "../../Rendering/Camera.h"
#include "../../Rendering/Texture.h"
#include "../../Common/Shader.h"
#include "../../Common/Parser.h"

class Water
{
    public:
        Water();
        void GenerateWaterVertices(Parser* EngineParser);
        void GenerateWaterBuffers();
        void RenderWater(Camera* EngineCamera,unsigned int SkyboxCubemapTexture);
        void CreateReflectionFrameBuffer();
        void Destroy();

        bool                        ToggleFrameBuffer = false;
    private:

        int                         WaterWidth  = 800;
        int                         WaterHeignt = 600;
        glm::mat4                   ReflectedView;
        Shader                      WaterShader;
        Texture                     WaterTexture;
        std::vector<float>          WaterVertices;
        std::vector<unsigned int>   WaterIndices;
        unsigned int                WaterVertexArray;
        unsigned int                WaterElementBuffer;
        unsigned int                WaterVertexBuffer;
        unsigned int                ReflectionFrameBuffer;
        unsigned int                ReflectionTexture;
        unsigned int                ReflectionDepthBuffer;
        float                       DeltaTimeValue;
};

#endif