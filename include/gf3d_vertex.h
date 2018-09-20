#ifndef __VERTEX_H
#define __VERTEX_H

#include <vulkan/vulkan.h>
#include "gf3d_types.h"

struct Vertex {
    int one;
};

/**
 * @brief Initialize a vertex buffer
 * 
 */
void gf3d_vertex_init();

/**
 * @brief Delete all vertex buffers
 * 
 */
void gf3d_vertex_shutdown();



#endif