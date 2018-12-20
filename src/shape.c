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

float shape_3d_edge_length(Vector3D e1, Vector3D e2)
{
  return vector3d_magnitude(vector3d(e1.x-e2.x,e1.y-e2.y,e1.z-e2.z));
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

Uint8 shape_point_in_cube(Vector3D p,Cube c)
{
    if ((p.x >= c.x) && (p.x <= c.x + c.width)&&
        (p.y >= c.y) && (p.y <= c.y + c.height)&&
        (p.z >= c.z) && (p.z <= c.z + c.depth))
        return 1;
    return 0;
}


//TRIANGLE START

void shape_triangle_get_normal(Triangle tri, Vector3D *normal)
{
    Vector3D t1,t2,t3;
    t1 = tri.p1;
    t2 = tri.p2;
    t3 = tri.p3;
    if (normal)
    {
        normal->x = (t1.y * (t2.z - t3.z)) + (t2.y * (t3.z - t1.z)) + (t3.y * (t1.z - t2.z));
        normal->y = (t1.z * (t2.x - t3.x)) + (t2.z * (t3.x - t1.x)) + (t3.z * (t1.x - t2.x));
        normal->z = (t1.x * (t2.y - t3.y)) + (t2.x * (t3.y - t1.y)) + (t3.x * (t1.y - t2.y));
        vector3d_normalize(normal);
    }
}

void shape_triangle_get_plane(Triangle tri, Vector3D *normal, float *D)
{
    shape_triangle_get_normal(tri, normal);
    Vector3D t1,t2,t3;
    t1 = tri.p1;
    t2 = tri.p2;
    t3 = tri.p3;
    if (D)
    {
        *D = -((t1.x *(t2.y * t3.z - t3.y * t2.z)) + 
            (t2.x *(t3.y * t1.z - t1.y * t3.z)) + 
            (t3.x *(t1.y * t2.z - t2.y * t1.z)));
    }
}

float shape_ray_in_plane(
    Vector3D start,
    Vector3D dir,
    Vector3D normal,
    float D,
    Vector3D *contact)
{
  float denom,tri;
  denom = ((normal.x * dir.x) + (normal.y * dir.y) + (normal.z * dir.z));
  if(denom == 0)return 0;
  tri = - (((normal.x * start.x) + (normal.y * start.y) + (normal.z * start.z) + D) / denom);
  if((tri > 0)&&(tri <= 1))
  {
    if (contact)
    {
      contact->x = start.x + (dir.x * tri);
      contact->y = start.y + (dir.y * tri);
      contact->z = start.z + (dir.z * tri);
    }
    return 1;
  }
  if (contact)
  {
    contact->x = start.x + (dir.x * tri);
    contact->y = start.y + (dir.y * tri);
    contact->z = start.z + (dir.z * tri);
  }
  return -1;
}

float shape_ray_cross_edge(
  Vector2D start,
  Vector2D v,
  Vector2D e1,
  Vector2D e2,
  Vector3D *contact)
{
  float testx, testy;
  float endx, endy;
  float Ua,Ub,Uden;
  
  endx = start.x + v.x;
  endy = start.y + v.y;
  
  Uden = ((e2.y - e1.y)*(endx - start.x)) - ((e2.x - e1.x)*(endy - start.y));
  if(Uden == 0)
  {
    return 0;/*parallel, can't hit*/
  }
    
  Ua = (((e2.x - e1.x)*(start.y - e1.y))-((e2.y - e1.y)*(start.x - e1.x))) / Uden;
  Ub = (((endx - start.x)*(start.y - e1.y))-((endy - start.y)*(start.x - e1.x))) / Uden;
  
  testx = start.x + (Ua * (endx - start.x));
  testy = start.y + (Ua * (endy - start.y));
  
  if(contact != NULL)
  {
    contact->x = testx;
    contact->y = testy;
  }
  
  if((Ua >= 0) && (Ua <= 1) && (Ub >= 0) && ( Ub <= 1))
  {
    return 1;
  }
  return 0;  
}

float shape_3d_point_in_triangle(
  Vector3D point,
  Triangle tri,
  Vector3D *normal)
{
    Vector3D n;
    Vector2D rayTest = {0,0};
    Uint32 intersectCount = 0;
    Vector3D t1,t2,t3;
    t1 = tri.p1;
    t2 = tri.p2;
    t3 = tri.p3;
    if (!normal)
    {
        shape_triangle_get_normal(tri, &n);
        normal = &n;
    }

    rayTest.y = shape_3d_edge_length(t1,t2) + shape_3d_edge_length(t2,t3) + shape_3d_edge_length(t3,t1);
    
    if (normal->x > (MAX(normal->y,normal->z)))
    {
        /*project triangle to yz plane*/
        if (shape_ray_cross_edge(
        vector2d(point.y,point.z),
        rayTest,
        vector2d(t1.y,t1.z),
        vector2d(t2.y,t2.z),
        NULL))
        {
        intersectCount++;
        }
        if (shape_ray_cross_edge(
        vector2d(point.y,point.z),
        rayTest,
        vector2d(t2.y,t2.z),
        vector2d(t3.y,t3.z),
        NULL))
        {
        intersectCount++;
        }
        if (shape_ray_cross_edge(
        vector2d(point.y,point.z),
        rayTest,
        vector2d(t3.y,t3.z),
        vector2d(t1.y,t1.z),
        NULL))
        {
        intersectCount++;
        }    
    }
    else if (normal->y > (MAX(normal->x,normal->z)))
    {
        /*project triangle to xz plane*/
        if (shape_ray_cross_edge(
        vector2d(point.x,point.z),
        rayTest,
        vector2d(t1.x,t1.z),
        vector2d(t2.x,t2.z),
        NULL))
        {
        intersectCount++;
        }
        if (shape_ray_cross_edge(
        vector2d(point.x,point.z),
        rayTest,
        vector2d(t2.x,t2.z),
        vector2d(t3.x,t3.z),
        NULL))
        {
        intersectCount++;
        }
        if (shape_ray_cross_edge(
        vector2d(point.x,point.z),
        rayTest,
        vector2d(t3.x,t3.z),
        vector2d(t1.x,t1.z),
        NULL))
        {
        intersectCount++;
        }
    }
    else
    {
        /*project triangle to xy plane*/
        if (shape_ray_cross_edge(
        vector2d(point.x,point.y),
        rayTest,
        vector2d(t1.x,t1.y),
        vector2d(t2.x,t2.y),
        NULL))
        {
        intersectCount++;
        }
        if (shape_ray_cross_edge(
        vector2d(point.x,point.y),
        rayTest,
        vector2d(t2.x,t2.y),
        vector2d(t3.x,t3.y),
        NULL))
        {
        intersectCount++;
        }
        if (shape_ray_cross_edge(
        vector2d(point.x,point.y),
        rayTest,
        vector2d(t3.x,t3.y),
        vector2d(t1.x,t1.y),
        NULL))
        {
        intersectCount++;
        }
    }
    if (intersectCount % 2)
    {
        return 1;
    }
    return 0;
}

float shape_3d_ray_in_triangle(
    Vector3D start,
    Vector3D dir,
    Triangle tri,
    Vector3D *contact)
{
    float t;
    Vector3D normal;
    float D;
    Vector3D intersectPoint;
    Vector3D t1,t2,t3;
    t1 = tri.p1;
    t2 = tri.p2;
    t3 = tri.p3;
  
    shape_triangle_get_plane(tri,&normal,&D);
    t = shape_ray_in_plane(
        start,
        dir,
        normal,
        D,
        &intersectPoint);
    if ((t <= 0)|| (t > 1))
    {
        return 0;
    }
    if (shape_3d_point_in_triangle(
        intersectPoint,
        tri,
        &normal))
    {
        if (contact)
        {
        vector3d_copy((*contact),intersectPoint);
        }
        return 1;
    }
    return 0;
}

//TRIANGLE END



Uint8 shape_sphere_cube_overlap(Sphere s, Cube c){
    int i,j,k;
    int width, height, depth;
    
    //TODO: Make this loop into a preprocessor command
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            for(k = 0; k < 2; k++)
            {
                width = i * c.width;
                height = j* c.height;
                depth = k * c.depth;
                if (shape_point_in_sphere(vector3d(c.x+width,c.y+height,c.z+depth),s))
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

Uint8 shape_edge_intersect(Edge a, Edge b){
    float Ua,Ub,Uden;
    Vector3D cross;
    vector3d_cross_product(&cross, 
                           vector3d(b.x2 - b.x2,
                                    b.y2 - b.y1,
                                    b.z2 - b.z1),
                           vector3d(a.x2 - a.x1,
                                    a.y2 - a.y1,
                                    a.z2 - a.y1));
    Uden = ((b.y2 - b.y1)*(a.x2 - a.x1)) - ((b.x2 - b.x1)*(a.y2 - a.y1));
    if(Uden == 0)
    {
        return 0;/*parallel, can'tri hit*/
    }

    Ua = (((b.x2 - b.x1)*(a.y1 - b.y1))-((b.y2 - b.y1)*(a.x1 - b.x1))) / Uden;
    Ub = (((a.x2 - a.x1)*(a.y1 - b.y1))-((a.y2 - a.y1)*(a.x1 - b.x1))) / Uden;

  
    if((Ua >= 0) && (Ua <= 1) && (Ub >= 0) && ( Ub <= 1))
    {
        return 1;
    }

    return 0;
}

Uint8 shape_edge_cube_overlap(Edge e, Cube c){
    Uint8 ret = 0;
    int i,j,k;
    int width, height, depth;
    
    //TODO: Make this loop into a preprocessor command
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            for(k = 0; k < 2; k++)
            {
                width = i * c.width;
                height = j* c.height;
                depth = k * c.depth;
                if (shape_point_in_cube(vector3d(c.x+width,c.y+height,c.z+depth),c))
                {
                    return 1;
                }
            }
        }
    }
    
    if ((shape_point_in_cube(vector3d(e.x1,e.y1, e.z1),c))||
        (shape_point_in_cube(vector3d(e.x2,e.y2, e.z2),c)))
    {
        // if either end point is within the rect, we have a collision
        return 1;
    }
    return 0;
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
                    return shape_edge_cube_overlap(b.shape.edge, a.shape.cube);
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