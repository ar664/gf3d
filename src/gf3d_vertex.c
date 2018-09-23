#include "gf3d_vertex.h"
#include "simple_logger.h"

struct VkVertexInputAttributeDescription   sampleAttributeDescription[2];
struct VkVertexInputBindingDescription     sampleBindingDescription;

typedef struct 
{
    VkDevice                            device;
    VkBuffer                            *vertexBuffer;
    VkDeviceMemory                      *vertexMemory;

}VertexBufferManager;

static VertexBufferManager gf3d_vertex_manager = {0};
uint32_t gf3d_find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties *memProperties);

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

void gf3d_vertex_create_buffer(VkPhysicalDevice device, VkBuffer *vertex_buffer, VkDeviceMemory *vertex_buffer_memory){
    VkBufferCreateInfo                  bufferInfo;
    VkMemoryAllocateInfo                allocInfo;
    VkMemoryRequirements                memRequirements;
    VkPhysicalDeviceMemoryProperties    memProperties;
    uint32_t                            typeBits;
    void*                               data;

    if(!vertex_buffer || !vertex_buffer_memory)
    {
        slog("Gave null vertex buffer");
        return;
    }

    *vertex_buffer = &sampleVerts;
    gf3d_vertex_manager.device = device;
    

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = (sizeof(Vector2D) + sizeof(Vector3D)) * 3;    /*< HARD CODED VALUE, EDIT LATER */
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if(vkCreateBuffer(device,
                      &bufferInfo, 
                      NULL, 
                      vertex_buffer) != VK_SUCCESS)
    {
        slog("Failed to create vertex buffer");
        return;
    }

    vkGetBufferMemoryRequirements(device, vertex_buffer, &memRequirements);

    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = gf3d_find_memory_type(memRequirements.memoryTypeBits,
                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                      &memProperties);
    if(allocInfo.memoryTypeIndex == 5077){
        slog("Failed to find memory type!");
        return;
    }


    if(vkAllocateMemory(device,
                        &allocInfo,
                        NULL,
                        vertex_buffer_memory) != VK_SUCCESS)
    {
        slog("Failed to allocate memory for vertex array");
        return;
    }

    if(vkBindBufferMemory(device,
                          vertex_buffer,
                          vertex_buffer_memory,
                          0) != VK_SUCCESS)
    {
        slog("Failed to bind memory");
        return;
    }

    vkMapMemory(device, vertex_buffer_memory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertex_buffer->data, (size_t) bufferInfo.size);
    vkUnmapMemory(device, vertex_buffer_memory);

    gf3d_vertex_manager.vertexBuffer = vertex_buffer;
    gf3d_vertex_manager.vertexMemory = vertex_buffer_memory;

    atexit(gf3d_vertex_shutdown);
    return;
}

uint32_t gf3d_find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties *memProperties){
    int i;
    vkGetPhysicalDeviceMemoryProperties(gf3d_vertex_manager.device, memProperties);
    
    for(i = 0; i < memProperties->memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties->memoryTypes[i].propertyFlags & properties) == properties) 
        {
            return i;
        }
    }
    return 5077;    //ERROR CODE
}

void gf3d_vertex_shutdown(){
    
    vkDestroyBuffer(gf3d_vertex_manager.device, *gf3d_vertex_manager.vertexBuffer, NULL);
    vkFreeMemory(gf3d_vertex_manager.device, *gf3d_vertex_manager.vertexMemory, NULL);
}