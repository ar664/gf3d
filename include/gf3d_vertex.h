#ifndef __VERTEX_H
#define __VERTEX_H

#include <vulkan/vulkan.h>
#include "gf3d_vector.h"

struct Vertex {
    Vector2D pos;
    Vector3D color;

};

extern struct VkVertexInputBindingDescription sampleBindingDescription;
extern struct VkVertexInputAttributeDescription sampleAttributeDescription[2];

extern struct Vertex sampleVerts[];

/**
 * @brief Initialize Binding & Attribute Descriptions
 * 
 */
void gf3d_vertex_init();

/**
 * @brief Create a vertex buffer with previously
 * 
 * @param device                The device to reference 
 * @param vertex_buffer         The buffer to fill with info about itself
 * @param vertex_buffer_memory  The vkBuffer to use for memory
 */
void gf3d_vertex_create_buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer *vertexBuffer, VkDeviceMemory *vertexBufferMemory);

/**
 * @brief Delete all vertex buffers
 * 
 */
void gf3d_vertex_shutdown();



#endif