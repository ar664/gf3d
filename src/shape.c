#include "shape.h"
#include "simple_logger.h"
#include "simple_json.h"


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

shape_t shape_sphere(float x, float y, float z, float radius){
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
    Vector3D v;
    vector3d_set(v, a.x - b.x, a.y - b.y, a.z - b.z);

    if(vector3d_magnitude_compare(v, a.radius + b.radius ) <= 0)
    {
        return 1;
    }
    return 0;
}

Uint8 shape_point_in_sphere(Vector3D p,Sphere c)
{
    if (vector3d_magnitude_compare(vector3d(c.x-p.x,c.y-p.y,c.z-p.z),c.radius ) <= 0)
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
                if (shape_point_in_sphere(vector3d(b.x+width,b.y+height,b.z+depth),a))
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

Uint8 shape_shape_overlap(shape_t a, shape_t b){
    switch(a.type){
        case(ST_SPHERE):
            switch(b.type){
                case(ST_CUBE):
                    return shape_sphere_cube_overlap(a.shape.sphere, b.shape.cube);
                    break;
                case(ST_SPHERE):
                    return shape_sphere_overlap(a.shape.sphere, b.shape.sphere);
                    break;
                case(ST_EDGE):
                    return 0;
                    break;
                default:
                    return 0;
                    break;
            }
            break;
        case(ST_CUBE):
            switch(b.type){
                case(ST_SPHERE):
                    return shape_sphere_cube_overlap(b.shape.sphere, a.shape.cube);
                    break;
                case(ST_CUBE):
                    return shape_cube_overlap(b.shape.cube, a.shape.cube);
                    break;
                case(ST_EDGE):
                    return 0;
                    break;
                default:
                    return 0;
                    break;
            }
            break;
        default:
            return 0;
    }
    return 0;
}

//Adding json/*.json to filename
//Expecting
//{
//"type" : "shape",
//"shape": [dimensions] CUBE: w,h,d SPHERE: r
//}
shape_t shape_load(char *filename){
    SJson *json, *typeJ, *shapeJ;
    char fn[128];
    char *value;
    shape_t shape = {0};

    sprintf(fn, "json/%s.json", filename);
    json = sj_load(fn);

    if(!json)
    {
        sj_free(json);
        return shape;
    }

    typeJ = sj_object_get_value(json, "type");
    
    if(!typeJ)
    {
        sj_free(json);
        return shape;
    }

    value = sj_get_string_value(typeJ);
    if(strcmp(SHAPE_STR_CUBE, value) == 0)
    {
        shape.type = ST_CUBE;
    } else if (strcmp(SHAPE_STR_SPHERE, value) == 0) 
    {
        shape.type = ST_SPHERE;
    } else if (strcmp(SHAPE_STR_EDGE, value) == 0)
    {
        shape.type = ST_EDGE;
    } else 
    {
        sj_free(json);
        return shape;
    }

    shapeJ = sj_object_get_value(json, "shape");

    if( !sj_is_array(shapeJ) )
    {
        sj_free(json);
        return shape;
    }


    if(shape.type == ST_CUBE){
        if(sj_array_get_count(shapeJ) != 3)
        {
            sj_free(json);
            return shape;
        }
        sj_get_float_value(sj_array_get_nth(shapeJ, 0),&shape.shape.cube.width);
        sj_get_float_value(sj_array_get_nth(shapeJ, 1),&shape.shape.cube.height);
        sj_get_float_value(sj_array_get_nth(shapeJ, 2),&shape.shape.cube.depth);
    } else if(shape.type == ST_SPHERE){
        if(sj_array_get_count(shapeJ) != 1)
        {
            sj_free(json);
            return shape;
        }
        sj_get_float_value(sj_array_get_nth(shapeJ, 0),&shape.shape.sphere.radius);
    }

    sj_free(json);
    return shape;
}