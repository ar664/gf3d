#ifndef __SHAPE_H
#define __SHAPE_H

#include "gf3d_matrix.h"
#include "gf3d_types.h"

typedef struct shape_s  shape_t;

typedef struct
{
    Vector3D p1;
    Vector3D p2;
    Vector3D p3;
}Triangle;

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
    float width;
    float height;
    float depth;
}Cube;

typedef enum
{
    ST_CUBE,
    ST_SPHERE,
    ST_EDGE
}ShapeTypes;

#define SHAPE_STR_CUBE   "CUBE"
#define SHAPE_STR_SPHERE "SPHERE"
#define SHAPE_STR_EDGE   "EDGE"

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
shape_t shape_cube(float x, float y, float z, float height, float width, float depth);

/**
 * @brief       Make a physics shape of a sphere
 * 
 * @param x         X position
 * @param y         Z position
 * @param z         Z position
 * @param radius    The radius of the sphere
 * @return shape_t  The shape with all the values set 
 */
shape_t shape_sphere(float x, float y, float z, float radius);

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
shape_t shape_edge(float x1, float y1, float z1, float x2, float y2, float z2);

/**
 * @brief Check if two spheres overlap
 * 
 * @param a     Sphere a
 * @param b     Sphere b
 * @return Uint8 True or False
 */
Uint8 shape_sphere_overlap(Sphere a, Sphere b);

/**
 * @brief Check if a sphere and a cube overlap
 * 
 * @param s     Sphera
 * @param c     Cube
 * @return Uint8 True or False
 */
Uint8 shape_sphere_cube_overlap(Sphere s, Cube c);

/**
 * @brief Check if two cubes overlap
 * 
 * @param a     Cube a
 * @param b     Cube b
 * @return Uint8 True of False
 */
Uint8 shape_cube_overlap(Cube a, Cube b);

/**
 * @brief Check if a 3D point is inside a sphere.
 * 
 * @param p 
 * @param c 
 * @return Uint8 True of False
 */
Uint8 shape_point_in_sphere(Vector3D p,Sphere c);

/**
 * @brief Check if a 3D point is inside a cube
 * 
 * @param p     Point
 * @param c     Cube
 * @return Uint8 True or False
 */
Uint8 shape_point_in_cube(Vector3D p, Cube c);

Uint8 shape_edge_intersect(Edge a, Edge b);

Uint8 shape_edge_cube_overlap(Edge e, Cube c);

void shape_triangle_get_normal(Triangle tri, Vector3D *normal);
void shape_triangle_get_plane(Triangle tri, Vector3D *normal, float *D);
float shape_ray_in_plane(
    Vector3D start,
    Vector3D dir,
    Vector3D normal,
    float D,
    Vector3D *contact);
float shape_3d_ray_in_triangle(
    Vector3D start,
    Vector3D dir,
    Triangle tri,
    Vector3D *contact);

/**
 * @brief Check whether two shapes are overlaped
 * 
 * @param shape a
 * @param shape b
 * @return Uint8 True or False
 */
Uint8 shape_shape_overlap(shape_t a, shape_t b);

/**
 * @brief Load a shape from a json fil=e
 * 
 * @param filename  The file name of the model Ex: cube.json would just be "cube"
 *
 * @return shape_t  The shape of the file 
 * 
 * @note    Shapes are in [x,y,z,...] order from json file
 */
shape_t shape_load(char *filename);

#endif