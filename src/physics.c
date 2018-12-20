#include "physics.h"
#include "camera.h"
#include "simple_logger.h"

//Insert O(k), Remove O(nlog(n)) 
body_t bodyList[PHYSICS_BODY_MAX] = {0};

int bodyCount = 0;
Vector3D cubeVerts[8];
Triangle cubeFaces[12];

void physics_system_init(){
    int x,y,z;
    for(x = 0; x < 2; x++)
    {
        for(y = 0; y < 2; y++)
        {
            for(z = 0; z < 2; z++)
            {
                cubeVerts[x*4+y*2+z] = vector3d(x ? -1 : 1,
                                            y ? -1 : 1,
                                            z ? -1 : 1);
            }
        }
    }

    cubeFaces[0] = (Triangle) {cubeVerts[4], cubeVerts[0], cubeVerts[3]};
    cubeFaces[1] = (Triangle) {cubeVerts[4], cubeVerts[3], cubeVerts[7]};
    cubeFaces[2] = (Triangle) {cubeVerts[0], cubeVerts[1], cubeVerts[2]};
    cubeFaces[3] = (Triangle) {cubeVerts[0], cubeVerts[2], cubeVerts[3]};
    cubeFaces[4] = (Triangle) {cubeVerts[1], cubeVerts[5], cubeVerts[6]};
    cubeFaces[5] = (Triangle) {cubeVerts[1], cubeVerts[6], cubeVerts[2]};
    cubeFaces[6] = (Triangle) {cubeVerts[5], cubeVerts[4], cubeVerts[7]};
    cubeFaces[7] = (Triangle) {cubeVerts[5], cubeVerts[7], cubeVerts[6]};
    cubeFaces[8] = (Triangle) {cubeVerts[7], cubeVerts[3], cubeVerts[2]};
    cubeFaces[9] = (Triangle) {cubeVerts[7], cubeVerts[2], cubeVerts[6]};
    cubeFaces[10] = (Triangle) {cubeVerts[0], cubeVerts[5], cubeVerts[1]};
    cubeFaces[11] = (Triangle) {cubeVerts[0], cubeVerts[4], cubeVerts[5]};

    memset(bodyList, 0, sizeof(body_t)*PHYSICS_BODY_MAX);
}

void physics_system_update(){
    int i, j;
    int collision, bodyCheck;
    entity_t *ent;

    //Move Objects
    for(i = 0; i < bodyCount; i++){
        if(bodyList[i].ent == NULL)
        {
            continue;
        }
        ent = bodyList[i].ent;

        //Velocity += acceleration + friction
        vector3d_add(ent->velocity,
                     ent->acceleration,
                     ent->velocity);
        //vector3d_add(ent->velocity,
        //             ent->velocity,
        //             friction);

        vector3d_add(ent->pos, ent->pos, ent->velocity);

        ent->shape.shape.sphere.x = ent->pos.x;
        ent->shape.shape.sphere.y = ent->pos.y;
        ent->shape.shape.sphere.z = ent->pos.z;
    }

    //Check for collisions
CHANGE:
    bodyCheck = bodyCount;
    for(i = 0; i < bodyCount; i++){
        for(j = bodyCount-1; j > 0; j--){
            if(i == j)
            {
                continue;
            }
            if(bodyList[i].ent && bodyList[j].ent)
            {
                collision = shape_shape_overlap(bodyList[i].ent->shape, bodyList[j].ent->shape);
                if(collision)
                {
                    //slog("Pos 1: %f, %f, %f", bodyList[i].ent->pos.x,bodyList[i].ent->pos.y,bodyList[i].ent->pos.z) ;
                    if(bodyList[i].ent->Touch){
                        bodyList[i].ent->Touch(bodyList[i].ent, bodyList[j].ent);
                    }
                    if(bodyCount != bodyCheck){
                        goto CHANGE;
                    }
                    if(bodyList[j].ent->Touch){
                        bodyList[j].ent->Touch(bodyList[j].ent, bodyList[i].ent);
                    }
                    if(bodyCount != bodyCheck){
                        goto CHANGE;
                    }
                }
            }
            
        }
    }
}

void physics_add_body(entity_t *ent){
    if(ent == NULL){
        return;
    }
    bodyList[bodyCount].ent = ent;
    ent->in_sim = 1;
    bodyCount++;
}

void physics_remove_body(entity_t *ent){
    int i;
    ent->in_sim = 0;
    for(i = 0; i < bodyCount; i++){
        if(ent == bodyList[i].ent){
            memset(&bodyList[i], 0, sizeof(body_t));
            memmove(&bodyList[i], &bodyList[i+1], (bodyCount-i-1)*sizeof(body_t));\
            break;
        }
    }
    bodyCount--;
}

entity_t *physics_raycast(Ray ray, Vector3D *contact){
    int i, faces;
    Triangle tri;
    Vector3D pos;
    shape_t shape;
    

    for(i = 0; i < bodyCount; i++)
    {
        if(bodyList[i].ent)
        {
            if(bodyList[i].ent->shape.type == ST_CUBE)
            {
                shape = bodyList[i].ent->shape;
                pos = vector3d( shape.shape.cube.x, shape.shape.cube.y, shape.shape.cube.z );
                for(faces = 0; faces < 12; faces++)
                {
                    tri = cubeFaces[faces];
                    vector3d_add(tri.p1, tri.p1, pos);
                    vector3d_add(tri.p2, tri.p2, pos);
                    vector3d_add(tri.p3, tri.p3, pos);

                    if(shape_3d_ray_in_triangle(ray.pos, ray.direction, tri, contact) == 1)
                    {
                        return bodyList[i].ent;
                    }
                }
                
                

            }
        }
    }
    return NULL;
}

entity_t *physics_raycast_point_on_screen(Point2D point, Vector3D *contact)
{
    Ray ray;
    ray.pos = camera_get_position();
    ray.direction = vector3d(0,0,-1);
    vector3d_set_magnitude(&ray.direction, 1000);

    return physics_raycast(ray, contact);
    
}
