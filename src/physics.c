#include "physics.h"



void physics_system_init(){
    return;
}

shape_t physics_shape_cube(float x, float y, float z, float height, float width, float depth){
    shape_t shape;
    shape.type = ST_CUBE;

    shape.shape.cube.x = x;
    shape.shape.cube.y = y;
    shape.shape.cube.z = z;

    shape.shape.cube.height = height;
    shape.shape.cube.width = width;
    shape.shape.cube.depth = depth;

    return shape;
}

shape_t physics_shape_sphere(float x, float y, float z, float radius);{
    shape_t shape;
    shape.type = ST_SPHERE;

    shape.shape.sphere.x = x;
    shape.shape.sphere.y = y;
    shape.shape.sphere.z = z;

    shape.shape.sphere.radius = radius;

    return shape;
}

shape_t physics_shape_edge(float x1, float y1, float z1, float x2, float y2, float z2){
    shape_t shape;
    shape.type = ST_EDGE;

    shape.shape.edge.x1 = x1;
    shape.shape.edge.y1 = y1;
    shape.shape.edge.z1 = z1;

    shape.shape.edge.x2 = x2;
    shape.shape.edge.y2 = y2;
    shape.shape.edge.z2 = z2;

    return shape;
}

