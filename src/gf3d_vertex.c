#include "gf3d_vertex.h"
#include "simple_logger.h"

struct VkVertexInputAttributeDescription   sampleAttributeDescription[2];
struct VkVertexInputBindingDescription     sampleBindingDescription;

struct Vertex sampleVerts[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

typedef struct 
{
    VkPhysicalDevice                    physicalDevice;
    VkDevice                            device;
    VkBuffer                            *vertexBuffer;
    VkDeviceMemory                      *vertexMemory;

}VertexBufferManager;

VkBufferCreateInfo                  bufferInfo;
VkMemoryAllocateInfo                allocInfo;
VkMemoryRequirements                memRequirements;
VkPhysicalDeviceMemoryProperties    memProperties;
VkBuffer vertexBufferGod;
static VertexBufferManager gf3d_vertex_manager = {0};
uint32_t gf3d_find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties *memProperties);

void gf3d_vertex_init(){
    sampleBindingDescription.binding = 0;
    sampleBindingDescription.stride = sizeof(struct Vertex);
    sampleBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    sampleAttributeDescription[0].binding = 0;
    sampleAttributeDescription[0].location = 0;
    sampleAttributeDescription[0].format = VK_FORMAT_R64G64_SFLOAT;
    sampleAttributeDescription[0].offset = offsetof(struct Vertex, pos);
    sampleAttributeDescription[1].binding = 0;
    sampleAttributeDescription[1].location = 0;
    sampleAttributeDescription[1].format = VK_FORMAT_R64G64B64_SFLOAT;
    sampleAttributeDescription[1].offset = offsetof(struct Vertex, color);

}

void gf3d_vertex_create_buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory){
    
    void*                               data;

    /* if(!vertexBuffer || !vertexBufferMemory)
    {
        slog("Gave null vertex buffer");
        return;
    } */
    
    gf3d_vertex_manager.device = device;
    gf3d_vertex_manager.physicalDevice = physicalDevice;

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(sampleVerts) * sizeof(sampleVerts[0]);    /*< HARD CODED VALUE, EDIT LATER */
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if(vkCreateBuffer(device,
                      &bufferInfo, 
                      NULL, 
                      vertexBuffer) != VK_SUCCESS)
    {
        slog("Failed to create vertex buffer");
        return;
    }

    vkGetBufferMemoryRequirements(device, *vertexBuffer, &memRequirements);

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
                        vertexBufferMemory) != VK_SUCCESS)
    {
        slog("Failed to allocate memory for vertex array");
        return;
    }

    if(vkBindBufferMemory(device,
                          *vertexBuffer,
                          *vertexBufferMemory,
                          0) != VK_SUCCESS)
    {
        slog("Failed to bind memory");
        return;
    }

    vkMapMemory(device, *vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, sampleVerts, (size_t) bufferInfo.size);
    vkUnmapMemory(device, *vertexBufferMemory);

    gf3d_vertex_manager.vertexBuffer = vertexBuffer;
    gf3d_vertex_manager.vertexMemory = vertexBufferMemory;

    atexit(gf3d_vertex_shutdown);
    return;
}

uint32_t gf3d_find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties *memProperties){
    int i;
    vkGetPhysicalDeviceMemoryProperties(gf3d_vertex_manager.physicalDevice, memProperties);

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
    
    vkDestroyBuffer(gf3d_vertex_manager.device, gf3d_vertex_manager.vertexBuffer, NULL);
    vkFreeMemory(gf3d_vertex_manager.device, gf3d_vertex_manager.vertexMemory, NULL);
}