#include "shape.h"



void system_init(){
    return;
}

shape_t shape_cube(float x, float y, float z, float height, float width, float depth){
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

shape_t shape_sphere(float x, float y, float z, float radius);{
    shape_t shape;
    shape.type = ST_SPHERE;

    shape.shape.sphere.x = x;
    shape.shape.sphere.y = y;
    shape.shape.sphere.z = z;

    shape.shape.sphere.radius = radius;

    return shape;
}

shape_t shape_edge(float x1, float y1, float z1, float x2, float y2, float z2){
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

Uint8 shape_sphere_overlap(Sphere a, Sphere b){
    Vector3d v;
    vector3d_set(v, a.x - b.x, a.y - b.y, c.x - c.y);

    if(vector3d_magnitude_compare(v, a.r + b.r) <= 0)
    {
        return 1;
    }
    return 0;
}

Uint8 shape_point_in_sphere(Vector3D p,Circle c)
{
    if (vector3d_magnitude_compare(vector3d(c.x-p.x,c.y-p.y,c.z-p.z),c.r) <= 0)
    {
        return 1;
    }
    return 0;
}

Uint8 shape_sphere_cube_overlap(Sphere a, Cube b){
    int i,j,k;
    int width, height, depth;
    
    //TODO: Make this loop into a preprocessor command
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            for(k = 0; k < 2; k++)
            {
                width = i * b.width;
                height = j* b.height;
                depth = k * b.depth;
                if (shape_point_in_sphere(vector2d(b.x+width,b.y+height,b.z+depth),a))
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

Uint8 shape_cube_overlap(Cube a, Cube b){
    if ((a.x > b.x+b.width)||
        (b.x > a.x+a.width)||
        (a.y > b.y+b.height)||
        (b.y > a.y+a.height)||
        (a.z > b.z+b.depth)||
        (b.z > b.z+b.depth))
    {
        return 0;
    }
    return 1;
}