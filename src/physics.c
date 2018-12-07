#include "physics.h"
#include "simple_logger.h"

//Insert O(k), Remove O(nlog(n)) 
body_t bodyList[PHYSICS_BODY_MAX] = {0};

int bodyCount = 0;


void physics_system_init(){
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


