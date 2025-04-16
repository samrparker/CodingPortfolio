#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include "GraphicsManager.h"
#include "Types.h"

template< typename T > constexpr const T* to_ptr(const T& val) { return &val; }
template< typename T, std::size_t N > constexpr const T* to_ptr(const T(&& arr)[N]) { return arr; }

using namespace glm;

namespace engineSpace {

    //Initializes the WebGPU instance variables in the order: 
    //WGPUInstance thisGPUInstance, WGPUSurface thisGPUSurface, WGPUAdapter thisGPUAdapter, WGPUDevice thisGPUDevice, WGPUQueue thisGPUQueue
    void GraphicsManager::InitWebGPU() {
        //Initializes the checks for the initialization of the WebGPU variable
        thisGPUInstance = wgpuCreateInstance(to_ptr(WGPUInstanceDescriptor{}));

        if (thisGPUInstance == nullptr) {
            std::cout << "UHHHHH WebGPU instance creation failed!" << std::endl;
            glfwTerminate();
        }

        //Inits the instance variable for the WebGPU's surface
        thisGPUSurface = glfwCreateWindowWGPUSurface(thisGPUInstance, thisWindow);

        //Inits the instance variable for the WebGPU's adapter
        thisGPUAdapter = nullptr;
        wgpuInstanceRequestAdapter(
            thisGPUInstance,
            to_ptr(WGPURequestAdapterOptions{ .compatibleSurface = thisGPUSurface }),
            [](WGPURequestAdapterStatus status, WGPUAdapter thisGPUAdapter, char const* message, void* adapter_ptr) {
                if (status != WGPURequestAdapterStatus_Success) {
                    std::cerr << "Failed to get a WebGPU adapter: " << message << std::endl;
                    glfwTerminate();
                }

                *static_cast<WGPUAdapter*>(adapter_ptr) = thisGPUAdapter;
            },
            &(thisGPUAdapter)
        );

        thisGPUDevice = nullptr;
        wgpuAdapterRequestDevice(
            thisGPUAdapter,
            nullptr,
            [](WGPURequestDeviceStatus status, WGPUDevice thisGPUDevice, char const* message, void* device_ptr) {
                if (status != WGPURequestDeviceStatus_Success) {
                    std::cerr << "Failed to get a WebGPU device: " << message << std::endl;
                    glfwTerminate();
                }

                *static_cast<WGPUDevice*>(device_ptr) = thisGPUDevice;
            },
            &(thisGPUDevice)
        );

        // An error callback to help with debugging
        wgpuDeviceSetUncapturedErrorCallback(
            thisGPUDevice,
            [](WGPUErrorType type, char const* message, void*) {
                std::cerr << "WebGPU uncaptured error type " << int(type) << " with message: " << message << std::endl;
            },
            nullptr
        );

        thisGPUQueue = wgpuDeviceGetQueue(thisGPUDevice);
    }

    //Releases or frees all the WebGPU isntance varables in the order (just the reverse initialization order):
    //WGPUQueue thisGPUQueue, WGPUDevice thisGPUDevice, WGPUAdapter thisGPUAdapter, WGPUSurface thisGPUSurface, WGPUInstance thisGPUInstance
    void GraphicsManager::WebGPUShutdown() {
        wgpuQueueRelease(thisGPUQueue);
        wgpuDeviceRelease(thisGPUDevice);
        wgpuAdapterRelease(thisGPUAdapter);
        wgpuSurfaceRelease(thisGPUSurface);
        wgpuInstanceRelease(thisGPUInstance);
        wgpuBufferRelease(thisGPUVertexBuffer);
        wgpuBufferRelease(thisGPUUniformBuffer);
        wgpuSamplerRelease(thisGPUSampler);
        wgpuShaderModuleRelease(thisShaderModule);
    }

    //Initializes the buffer and gives the buffer an example of the rectangle data
    void GraphicsManager::InitBuffers() {
        thisGPUVertexBuffer = wgpuDeviceCreateBuffer(thisGPUDevice, to_ptr(WGPUBufferDescriptor{
            .label = "Vertex Buffer",
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
            .size = sizeof(vertices)
            }));
        wgpuQueueWriteBuffer(thisGPUQueue, thisGPUVertexBuffer, 0, vertices, sizeof(vertices));

        //Inits the uninform construct 
        thisGPUUniformBuffer = wgpuDeviceCreateBuffer(thisGPUDevice, to_ptr(WGPUBufferDescriptor{
                .label = "Uniform Buffer",
                .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
                .size = sizeof(Uniforms)
            }));
    }

    //Prepares to init the pipeline by configuring the WebGPUSurface thisGPUSurface instance variable.
    void GraphicsManager::ConfigSurface() {
        int width, height;
        glfwGetFramebufferSize(thisWindow, &width, &height);
        wgpuSurfaceConfigure(thisGPUSurface, to_ptr(WGPUSurfaceConfiguration{
            .device = thisGPUDevice,
            .format = wgpuSurfaceGetPreferredFormat(thisGPUSurface, thisGPUAdapter),
            .usage = WGPUTextureUsage_RenderAttachment,
            .width = (uint32_t)width,
            .height = (uint32_t)height
            }));
    }

    //Inits the thisGPUSampler variable
    void GraphicsManager::CreateSampler() {
        thisGPUSampler = wgpuDeviceCreateSampler(thisGPUDevice, to_ptr(WGPUSamplerDescriptor{
            .addressModeU = WGPUAddressMode_ClampToEdge,
            .addressModeV = WGPUAddressMode_ClampToEdge,
            .magFilter = WGPUFilterMode_Linear,
            .minFilter = WGPUFilterMode_Linear,
            .maxAnisotropy = 1
            }));
    }

    void GraphicsManager::CreatePipeline() {
        const char* source = R"(
        struct Uniforms {
            projection: mat4x4f,
        };

        @group(0) @binding(0) var<uniform> uniforms: Uniforms;
        @group(0) @binding(1) var texSampler: sampler;
        @group(0) @binding(2) var texData: texture_2d<f32>;

        struct VertexInput {
            @location(0) position: vec2f,
            @location(1) texcoords: vec2f,
            @location(2) translation: vec3f,
            @location(3) scale: vec2f,
        };

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) texcoords: vec2f,
        };

        @vertex
        fn vertex_shader_main( in: VertexInput ) -> VertexOutput {
            var out: VertexOutput;
            out.position = uniforms.projection * vec4f( vec3f( in.scale * in.position, 0.0 ) + in.translation, 1.0 );
            out.texcoords = in.texcoords;
            return out;
        }

        @fragment
        fn fragment_shader_main( in: VertexOutput ) -> @location(0) vec4f {
            let color = textureSample( texData, texSampler, in.texcoords ).rgba;
            return color;
        }
        )";

        //Makes the shader module
        WGPUShaderModuleWGSLDescriptor code_desc = {};
        code_desc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        code_desc.code = source; // The shader source as a `char*`
        WGPUShaderModuleDescriptor shader_desc = {};
        shader_desc.nextInChain = &code_desc.chain;
        thisShaderModule = wgpuDeviceCreateShaderModule(thisGPUDevice, &shader_desc);

        //Actually creates the pipeline
        InitPipeline();
    }

    void GraphicsManager::InitPipeline() {

        //DON'T LOOK :(
        thisGPUPipeline = wgpuDeviceCreateRenderPipeline(thisGPUDevice, to_ptr(WGPURenderPipelineDescriptor{

            // Describe the vertex shader inputs
            .vertex = {
                .module = thisShaderModule,
                .entryPoint = "vertex_shader_main",
                // Vertex attributes.
                .bufferCount = 2,
                .buffers = to_ptr<WGPUVertexBufferLayout>({
                // We have one buffer with our per-vertex position and UV data. This data never changes.
                // Note how the type, byte offset, and stride (bytes between elements) exactly matches our `vertex_buffer`.
                {
                    .arrayStride = 4 * sizeof(float),
                    .attributeCount = 2,
                    .attributes = to_ptr<WGPUVertexAttribute>({
                        // Position x,y are first.
                        {
                            .format = WGPUVertexFormat_Float32x2,
                            .offset = 0,
                            .shaderLocation = 0
                        },
                        // Texture coordinates u,v are second.
                        {
                            .format = WGPUVertexFormat_Float32x2,
                            .offset = 2 * sizeof(float),
                            .shaderLocation = 1
                        }
                        })
                },
                    // We will use a second buffer with our per-sprite translation and scale. This data will be set in our draw function.
                    {
                        .arrayStride = sizeof(InstanceData),
                        // This data is per-instance. All four vertices will get the same value. Each instance of drawing the vertices will get a different value.
                        // The type, byte offset, and stride (bytes between elements) exactly match the array of `InstanceData` structs we will upload in our draw function.
                        .stepMode = WGPUVertexStepMode_Instance,
                        .attributeCount = 2,
                        .attributes = to_ptr<WGPUVertexAttribute>({
                        // Translation as a 3D vector.
                        {
                            .format = WGPUVertexFormat_Float32x3,
                            .offset = offsetof(InstanceData, translation),
                            .shaderLocation = 2
                        },
                            // Scale as a 2D vector for non-uniform scaling.
                            {
                                .format = WGPUVertexFormat_Float32x2,
                                .offset = offsetof(InstanceData, scale),
                                .shaderLocation = 3
                            }
                            })
                    }
                    })
                },

            // Interpret our 4 vertices as a triangle strip
            .primitive = WGPUPrimitiveState{
                .topology = WGPUPrimitiveTopology_TriangleStrip,
                },

                // No multi-sampling (1 sample per pixel, all bits on).
                .multisample = WGPUMultisampleState{
                    .count = 1,
                    .mask = ~0u
                    },

            // Describe the fragment shader and its output
            .fragment = to_ptr(WGPUFragmentState{
                .module = thisShaderModule,
                .entryPoint = "fragment_shader_main",

                // Our fragment shader outputs a single color value per pixel.
                .targetCount = 1,
                .targets = to_ptr<WGPUColorTargetState>({
                    {
                        .format = wgpuSurfaceGetPreferredFormat(thisGPUSurface, thisGPUAdapter),
                        // The images we want to draw may have transparency, so let's turn on alpha blending with over compositing (ɑ⋅foreground + (1-ɑ)⋅background).
                        // This will blend with whatever has already been drawn.
                        .blend = to_ptr(WGPUBlendState{
                        // Over blending for color
                        .color = {
                            .operation = WGPUBlendOperation_Add,
                            .srcFactor = WGPUBlendFactor_SrcAlpha,
                            .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha
                            },
                            // Leave destination alpha alone
                            .alpha = {
                                .operation = WGPUBlendOperation_Add,
                                .srcFactor = WGPUBlendFactor_Zero,
                                .dstFactor = WGPUBlendFactor_One
                                }
                            }),
                        .writeMask = WGPUColorWriteMask_All
                    }})
                })
            }));
    }

    void GraphicsManager::LoadSprite(const string& name, const string& path, float x, float y) {
        std::cout << "Adding Sprite!" << std::endl;
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

        //Creates the texture to pass into the GPU
        WGPUTexture tex = wgpuDeviceCreateTexture(thisGPUDevice, to_ptr(WGPUTextureDescriptor{
            .label = path.c_str(),
            .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
            .dimension = WGPUTextureDimension_2D,
            .size = { (uint32_t)width, (uint32_t)height, 1 },
            .format = WGPUTextureFormat_RGBA8UnormSrgb,
            .mipLevelCount = 1,
            .sampleCount = 1
        }));

        imageMap[name] = {};
        imageMap[name].imageWidth = width;
        imageMap[name].imageHeight = height;
        imageMap[name].imageTexture = tex;
        imageMap[name].imagePosition = std::vector{ x, y };
        imageMap[name].imageScale = 10;

        Sprite newSprite = Sprite{ name, 0.1f };
        sprites.push_back(newSprite);
        amountOfSprites++;
        std::cout << "New size: " << sprites.size() << std::endl;

        wgpuQueueWriteTexture(
            thisGPUQueue,
            to_ptr<WGPUImageCopyTexture>({ .texture = tex }),
            data,
            width * height * 4,
            to_ptr<WGPUTextureDataLayout>({ .bytesPerRow = (uint32_t)(width * 4), .rowsPerImage = (uint32_t)height }),
            to_ptr(WGPUExtent3D{ (uint32_t)width, (uint32_t)height, 1 })
        );
        stbi_image_free(data);
    }

    void GraphicsManager::MoveSprite(const string& spriteName, float moveX, float moveY) {
        std::vector imageCoords = imageMap[spriteName].imagePosition;
        imageMap[spriteName].imagePosition = std::vector{ imageCoords[0] + moveX, imageCoords[1] + moveY };
    }

    void GraphicsManager::Draw(const std::vector< Sprite >& sprites) {
        //Inits the instance buffer, needed in the drawing sprites part of the drawing section
        InitInstanceBuffer(sprites);

        //Step 1
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(thisGPUDevice, nullptr);

        //Step 2
        WGPUSurfaceTexture surface_texture{};
        thisGPUSurfaceTexture = surface_texture;
        wgpuSurfaceGetCurrentTexture(thisGPUSurface, &thisGPUSurfaceTexture);
        WGPUTextureView current_texture_view = wgpuTextureCreateView(thisGPUSurfaceTexture.texture, nullptr);

        //Step 3
        WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, to_ptr<WGPURenderPassDescriptor>({
            .colorAttachmentCount = 1,
            .colorAttachments = to_ptr<WGPURenderPassColorAttachment>({{
                .view = current_texture_view,
                .loadOp = WGPULoadOp_Clear,
                .storeOp = WGPUStoreOp_Store,
                .clearValue = WGPUColor{ 0.0, 0.0, 0.0, 1.0 }
                }})
            }));

        //Steps 4-6
        wgpuRenderPassEncoderSetPipeline(render_pass, thisGPUPipeline);
        wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0 /* slot */, thisGPUVertexBuffer, 0, 4 * 4 * sizeof(float));
        wgpuRenderPassEncoderSetVertexBuffer(render_pass, 1 /* slot */, thisGPUInstanceBuffer, 0, sizeof(InstanceData) * sprites.size());

        //Step 7: Uniforms struct
        Uniforms gameWorldBounds = InitUniformsStruct();
        wgpuQueueWriteBuffer(thisGPUQueue, thisGPUUniformBuffer, 0, &gameWorldBounds, sizeof(Uniforms));

        //Step 8: Sorting sprites
        //std::cout << "YO1" << std::endl;
        SortSprites(sprites);
        //Step 9: Drawing them
        //std::cout << "YO2" << std::endl;
        LoadSprites(sprites, render_pass);
        //std::cout << "YO3" << std::endl;
        //Step 10: Finish drawing
        wgpuRenderPassEncoderEnd(render_pass);

        WGPUCommandBuffer commandBuff = wgpuCommandEncoderFinish(encoder, nullptr);
        wgpuQueueSubmit(thisGPUQueue, 1, &commandBuff);
        wgpuSurfacePresent(thisGPUSurface);

        wgpuBufferRelease(thisGPUInstanceBuffer);
    }

    void GraphicsManager::InitInstanceBuffer(const std::vector< Sprite >& sprites) {
        thisGPUInstanceBuffer = wgpuDeviceCreateBuffer(thisGPUDevice, to_ptr<WGPUBufferDescriptor>({
            .label = "Instance Buffer",
            .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
            .size = sizeof(InstanceData) * sprites.size()
        }));
    }

    Uniforms GraphicsManager::InitUniformsStruct() {
        // Start with an identity matrix.
        Uniforms uniforms{};
        uniforms.projection = mat4{ 1 };
        // Scale x and y by 1/100.
        uniforms.projection[0][0] = uniforms.projection[1][1] = 1. / 100.;
        // Scale the long edge by an additional 1/(long/short) = short/long.
        if (windowWidth < windowHeight) {
            uniforms.projection[1][1] *= windowWidth;
            uniforms.projection[1][1] /= windowHeight;
        }
        else {
            uniforms.projection[0][0] *= windowHeight;
            uniforms.projection[0][0] /= windowWidth;
        }
        return uniforms;
    }

    const std::vector< Sprite > GraphicsManager::SortSprites(const std::vector< Sprite >& sprites) {
        auto copyOfSprites = sprites;
        std::sort(copyOfSprites.begin(), copyOfSprites.end(), [](const Sprite& lhs, const Sprite& rhs) { return lhs.imageZ > rhs.imageZ; });
        return copyOfSprites;
    }

    void GraphicsManager::LoadSprites(const std::vector<Sprite>& sprites, WGPURenderPassEncoder renderEncoder) {
        for (int index = 0; index < sprites.size(); index++) {

            //std::cout << "Loading sprite " << index+1 << std::endl;
            //Gets the data for the InstanceData
            InstanceData spriteData{};
            spriteData.translation = {};
            spriteData.scale = {};


            //std::cout << "Loading" << std::endl;
            Sprite spriteToLoad = sprites[index];

            ImageData imageToLoad = imageMap[spriteToLoad.imageName];

            if (imageToLoad.imageWidth < imageToLoad.imageHeight) {
                spriteData.scale = vec2(real(imageToLoad.imageWidth) / imageToLoad.imageHeight * imageToLoad.imageScale, 1.0 * imageToLoad.imageScale);
            }
            else {
                spriteData.scale = vec2(1.0 * imageToLoad.imageScale, real(imageToLoad.imageHeight) / imageToLoad.imageWidth * imageToLoad.imageScale);
            }

            spriteData.translation = vec3(imageToLoad.imagePosition[0], imageToLoad.imagePosition[1], spriteToLoad.imageZ);

            wgpuQueueWriteBuffer(thisGPUQueue, thisGPUInstanceBuffer, index * sizeof(InstanceData), &spriteData, sizeof(InstanceData));

            //std::cout << "Done writing to buffer" << std::endl;
            //Making the binding for the sprite
            auto layout = wgpuRenderPipelineGetBindGroupLayout(thisGPUPipeline, 0);
            WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(thisGPUDevice, to_ptr(WGPUBindGroupDescriptor{
                .layout = layout,
                .entryCount = 3,
                // The entries `.binding` matches what we wrote in the shader.
                .entries = to_ptr<WGPUBindGroupEntry>({
                    {
                        .binding = 0,
                        .buffer = thisGPUUniformBuffer,
                        .size = sizeof(Uniforms)
                    },
                    {
                        .binding = 1,
                        .sampler = thisGPUSampler,
                    },
                    {
                        .binding = 2,
                        .textureView = wgpuTextureCreateView(imageToLoad.imageTexture, nullptr)
                    }
                    })
                }));
            wgpuBindGroupLayoutRelease(layout);

            wgpuRenderPassEncoderSetBindGroup(renderEncoder, 0, bind_group, 0, nullptr);

            wgpuRenderPassEncoderDraw(renderEncoder, 4, 1, 0, index);
        }
    }

    std::vector<Sprite> GraphicsManager::SetAmountOfSprites(int spriteAmount) {
        return std::vector<Sprite>(spriteAmount);
    }

    void GraphicsManager::DrawSprites() {
        Draw(sprites);
    }

    void GraphicsManager::DestroyImage(ImageData imageToDestroy) {
        if (imageToDestroy.imageTexture != nullptr) {
            wgpuTextureDestroy(imageToDestroy.imageTexture);
            wgpuTextureRelease(imageToDestroy.imageTexture);
        }
        if (imageToDestroy.imageBindGroup != nullptr) {
            wgpuBindGroupRelease(imageToDestroy.imageBindGroup);
        }
    }

    void GraphicsManager::Startup() {
        amountOfSprites = 0;
        glfwInit();
        //Minor GLFW config. First is the ensure no API creation, second is to prevent resizes of the window
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        windowWidth = 500;
        windowHeight = 500;

        sprites = {};

        // Create the window.
        GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "KNACK", false ? glfwGetPrimaryMonitor() : 0, 0);
        this->thisWindow = window;
        if (!window)
        {
            std::cerr << "Failed to create a window." << std::endl;
            glfwTerminate();
        }
        glfwSetWindowAspectRatio(window, 16, 9);
        

        //check above function for exactly what this does, but this just initialized all the WebGPU instance variables in the GraphicsManager.h header.
        InitWebGPU();
        InitBuffers();
        ConfigSurface();
        CreateSampler();
        CreatePipeline();
    }

    GLFWwindow* GraphicsManager::GetWindow() {
        return thisWindow;
    }

    void GraphicsManager::Shutdown() {
        WebGPUShutdown();
        glfwTerminate();
    }

    void GraphicsManager::SetShouldQuit(bool shouldQuit) {
        glfwSetWindowShouldClose(this->thisWindow, shouldQuit);
    }

    bool GraphicsManager::ShouldQuit() {
        int close = glfwWindowShouldClose(this->thisWindow);

        return close == 1;
    }
}