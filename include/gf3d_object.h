#ifndef __GF3D_OBJECT_H
#define __GF3D_OBJECT_H

/**
 * obj
 * @license The MIT License (MIT)
 *   @copyright Copyright (c) 2015 EngineerOfLies
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *    The above copyright notice and this permission notice shall be included in all
 *    copies or substantial portions of the Software.
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#include <vulkan/vulkan.h>
#include "gf3d_vector.h"

typedef struct Truple_S
{
    uint32_t    v;  /**< Vertices */
    uint32_t    n;  /**< Normal */
    uint32_t    t;  /**< Texel */
}Truple;

typedef struct Triangle_S
{
    Truple p[3];
}Triangle;

typedef struct
{
    char        filename[512];
    
    uint32_t    num_triangles;
    uint32_t    num_vertices;
    uint32_t    num_texels;
    uint32_t    num_normals;

    double     *vertex_array;
    double     *texel_array;
    double     *normal_array;
    Triangle   *triangle_array;

    uint8_t     ref_count;

}Object;


/**
 * @brief initialize the object subsystem
 * 
 */
void gf3d_object_init();



/**
 * @brief load an object file into an obj
 * @param filename the object file to parse
 * @return NULL on failure or an Obj pointer otherwise;
 * 
 */
Object *gf3d_object_load(char *filename);

/**
 * @brief Free the objects children from memory and set it to 0.
 * 
 * @param object    The object to free.
 */
static void gf3d_object_delete(Object *object);

#endif