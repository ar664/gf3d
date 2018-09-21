#include "gf3d_vertex.h"
#include "simple_logger.h"

struct VkVertexInputAttributeDescription   sampleAttributeDescription[2];
struct VkVertexInputBindingDescription     sampleBindingDescription;

typedef struct 
{
    VkDevice                            device;
    VkBuffer                            vertexBuffer;
}VertexBufferManager;

static VertexBufferManager gf3d_vertex_buffer_manager = {0};

void gf3d_vertex_init(){
    sampleBindingDescription.binding = 0;
    sampleBindingDescription.stride = sizeof(struct Vertex);
    sampleBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    sampleAttributeDescription[0].binding = 0;
    sampleAttributeDescription[0].location = 0;
    sampleAttributeDescription[0].format = VK_FORMAT_R64G64B64A64_SFLOAT;
    sampleAttributeDescription[0].offset = offsetof(struct Vertex, pos);
    sampleAttributeDescription[1].binding = 0;
    sampleAttributeDescription[1].location = 0;
    sampleAttributeDescription[1].format = VK_FORMAT_R64G64B64A64_SFLOAT;
    sampleAttributeDescription[1].offset = offsetof(struct Vertex, color);

}

void gf3d_vertex_create_buffer(VkPhysicalDevice device, VkBuffer *vertex_buffer){
    
    if(!vertex_buffer)
    {
        slog("Gave null vertex buffer");
        return;
    }
    *vertex_buffer = &sampleVerts;
    gf3d_vertex_buffer_manager.device = device;
    gf3d_vertex_buffer_manager.vertexBuffer = vertex_buffer;
    VkBufferCreateInfo bufferInfo;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = (sizeof(Vector2D) + sizeof(Vector3D)) * 3;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if(vkCreateBuffer(gf3d_vertex_buffer_manager.device, &bufferInfo, NULL, vertex_buffer) != VK_SUCCESS){
        slog("Failed to create vertex buffer");
        return;
    }

    atexit(gf3d_vertex_shutdown);

    return;
}

void gf3d_vertex_shutdown(){
    
    vkDestroyBuffer(gf3d_vertex_buffer_manager.device,gf3d_vertex_buffer_manager.vertexBuffer, NULL);

}