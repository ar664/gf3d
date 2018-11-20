#ifndef __PHYSICS_H
#define __PHYSICS_H

#include "entity.h"

typedef struct shape_s  shape_t;
typedef struct body_s   body_t;

typedef struct
{
    float x1,y1,x2,y2,z1,z2;
}Edge;

typedef struct
{
    float       x,y,z;
    float       radius;
}Sphere;

typedef struct
{
    float x,y,z;
    float width,
    float height
    float depth;
}Cube;

typedef enum
{
    ST_CUBE,
    ST_SPHERE,
    ST_EDGE
}ShapeTypes;

struct shape_s{
    ShapeTypes type;
    union
    {
        Sphere sphere;
        Cube cube;
        Edge edge;
    }shape;
};

/**
 * @brief Starts the physics system for the entities
 * 
 */
void physics_system_init();

/**
 * @brief           Make a physics shape of a cube
 * 
 * @param x         X position
 * @param y         Y position
 * @param z         Z position
 * @param height    Height of cube
 * @param width     Width of cube
 * @param depth     Depth of cube
 * @return shape_t  The shape with all the values set
 */
shape_t physics_shape_cube(float x, float y, float z, float height, float width, float depth);

/**
 * @brief       Make a physics shape of a sphere
 * 
 * @param x         X position
 * @param y         Z position
 * @param z         Z position
 * @param radius    The radius of the sphere
 * @return shape_t  The shape with all the values set 
 */
shape_t physics_shape_sphere(float x, float y, float z, float radius);

/**
 * @brief       Make a physics shape of an edge
 * 
 * @param x1        
 * @param y1 
 * @param z1 
 * @param x2 
 * @param y2 
 * @param z2 
 * @return shape_t The shape with all values set 
 */
shape_t physics_shape_edge(float x1, float y1, float z1, float x2, float y2, float z2);

#endif