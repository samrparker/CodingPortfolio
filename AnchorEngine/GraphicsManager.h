#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <unordered_map>
#include "Types.h"
#include "glm/glm.hpp"

using namespace glm;

namespace engineSpace {
    struct ImageData {
        int imageHeight;
        int imageWidth;
        std::vector<float> imagePosition;
        float imageScale;
        WGPUTexture imageTexture;
        WGPUBindGroup imageBindGroup;
    };

    struct Sprite {
        string imageName;
        float imageZ;
    };


    struct InstanceData {
        vec3 translation;
        vec2 scale;
        // rotation?
    };

    struct Uniforms {
        mat4 projection;
    };

    class GraphicsManager {
    public:
        GLFWwindow* thisWindow;
        WGPUInstance thisGPUInstance;
        WGPUSurface thisGPUSurface;
        WGPUAdapter thisGPUAdapter;
        WGPUDevice thisGPUDevice;
        WGPUQueue thisGPUQueue;
        WGPUBuffer thisGPUVertexBuffer;
        WGPURenderPipeline thisGPUPipeline;
        WGPUBuffer thisGPUUniformBuffer;
        WGPUBuffer thisGPUInstanceBuffer;
        WGPUSampler thisGPUSampler;
        WGPUShaderModule thisShaderModule;
        WGPUSurfaceTexture thisGPUSurfaceTexture;

        //window information
        int windowWidth;
        int windowHeight;

        //Map to keep track of the images
        std::unordered_map<string, ImageData> imageMap;

        //Functions specifically resevred for usage with WebGPU stuff (initialization, drawing, etc.)
        void InitWebGPU();
        void WebGPUShutdown();
        void InitBuffers();
        void ConfigSurface();
        void CreateSampler();
        void CreatePipeline();
        void InitPipeline();

        void LoadSprite(const string& name, const string& path, float x, float y);
        void DestroyImage(ImageData imageToDestroy);

        void Draw(const std::vector< Sprite >& sprites);
        Uniforms InitUniformsStruct();
        const std::vector<Sprite> SortSprites(const std::vector<Sprite>& sprites);
        void InitInstanceBuffer(const std::vector< Sprite >& sprites);
        void LoadSprites(const std::vector<Sprite>& sprites, WGPURenderPassEncoder renderEncoder);

        std::vector<Sprite> SetAmountOfSprites(int spriteAmount);
        void DrawSprites();
        void MoveSprite(const string& name, float moveX, float moveY);

        std::vector<Sprite> sprites;
        std::vector<Sprite> uiSprites;
        int amountOfSprites;
        int drawAmount;

        GLFWwindow* GetWindow();


        //Generic functions
        void Startup();
        void Shutdown();
        void SetShouldQuit(bool shouldQuit);
        bool ShouldQuit();

    };
}
#endif