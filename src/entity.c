#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "physics.h"
#include "camera.h"
#include "simple_logger.h"
#include "SDL.h"
#include "gf3d_vgraphics.h"
#include "path.h"
#include "tile.h"

entity_t* entity_list;
Uint8 *entity_keys = NULL;
const Vector3D AXIS_X = {1,0,0};
const Vector3D AXIS_Y = {0,1,0};
const Vector3D AXIS_Z = {0,0,1};


void entity_generic_think(entity_t *self);
void entity_generic_draw(entity_t *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer);

void entity_system_shutdown();

void entity_system_init(){
    int i;
    entity_list = (entity_t*) malloc(sizeof(entity_t)*ENTITY_MAX);
    if(!entity_list){
        slog("Unable to allocate for entity_list");
        exit(-1);
        return;
    }
    for(i = 0; i < ENTITY_MAX; i++){
        memset(&entity_list[i], 0, sizeof(entity_t));
        
    }
    atexit(entity_system_shutdown);
}

entity_t *entity_new(){
    int i;
    for(i = 0; i < ENTITY_MAX;i++){
        if(!entity_list[i].in_use){
            entity_list[i].in_use = 1;
            entity_list[i].Destroy = &entity_generic_destroy;
            return &entity_list[i];
        }
    }
    return NULL;
}

entity_t *entity_load(char *model){
    entity_t *ent;
    ent = entity_new();
    if(!ent){
        return NULL;
    }
    ent->name = model;
    ent->model = gf3d_model_load(model);
    ent->shape = shape_load(model);
    ent->Think = NULL;//&entity_generic_think;
    gf3d_matrix_identity(ent->ubo.model);
    
    camera_get_perspective(ent->ubo.proj);
    camera_get_view(ent->ubo.view);

    vector3d_set(ent->scale, 1, 1, 1);

    return ent;
}

//THINK FUNCTIONS START 

void entity_think_test_path(entity_t *self){
    path_t *path;
    Point2D startPoint,randPoint;
    Uint32 x, y;
    if(!self){
        return;
    }
    path = (path_t*)self->extra_data;
    if(!path)
    {
        self->extra_data = malloc(sizeof(path_t));
        path = (path_t*) self->extra_data;
        startPoint.x = 0;
        startPoint.y = 0;
        randPoint.x = rand()%TILE_MAX_X;
        randPoint.y = rand()%TILE_MAX_Y;
        *path = path_get_path(startPoint, randPoint);
        path->current = 0;
    }
    if(self->think_next == 0)
    {
        if(path->current >= path->count || path->count == 0)
        {
            startPoint.x = path->path[path->count-1].x;
            startPoint.y = path->path[path->count-1].y;
            randPoint.x = rand()%TILE_MAX_X;
            randPoint.y = rand()%TILE_MAX_Y;
            path_free(path);
            *path = path_get_path(startPoint, randPoint);
            slog("Start point: %d %d End point: %d %d", startPoint.x, startPoint.y, randPoint.x, randPoint.y);
            path->current = 0;
            return;
        }
        x = path->path[path->current].x;
        y = path->path[path->current].y;
        self->pos = tile_get_real_position(x, y);
        path->current++;
        self->think_next = TILE_RESOURCE_TICK;
        slog("Pathed %d, x: %d , y: %d", path->current, path->path[path->current-1].x, path->path[path->current-1].y);
    }
    self->think_next--;
}

void entity_think_rotate_self_x(entity_t *self){
    int time;
    if(!self){
        slog("Tried to think NULL entity");
        return;
    }
    time = 0; //THINKING EVERY FRAME *TODO CHANGE*
    if(time > self->think_next){
        return;
    }

    self->relative_rotation.x += 0.005;
    vector3d_add(self->rotation, self->relative_rotation, self->rotation);  //Keeping track of overall rotation


}

void entity_think_generic(entity_t *self){
    int time;
    if(!self){
        slog("Tried to think NULL entity");
        return;
    }
    time = 0; //THINKING EVERY FRAME *TODO CHANGE*
    if(time > self->think_next){
        return;
    }

    if(entity_keys[SDL_SCANCODE_W]){
        self->pos.y += 0.005;
    } else if(entity_keys[SDL_SCANCODE_S]){
        self->pos.y -= 0.005;
    } else if(entity_keys[SDL_SCANCODE_A]){
        self->pos.x += 0.005;
    } else if(entity_keys[SDL_SCANCODE_D]){
        self->pos.x -= 0.005;
    }

    if(entity_keys[SDL_SCANCODE_X]){
        self->relative_rotation.x += 0.005;
    } else if(entity_keys[SDL_SCANCODE_Y]){
        self->relative_rotation.y += 0.005;
    } else if (entity_keys[SDL_SCANCODE_Z]){
        self->relative_rotation.z += 0.005;
    }

    vector3d_add(self->rotation, self->relative_rotation, self->rotation);  //Keeping track of overall rotation

    /* if(entity_keys[SDL_SCANCODE_E]){
        self->scale += 0.005;
    } else if (entity_keys[SDL_SCANCODE_Q]){
        self->scale -= 0.005;
    } */

    //self->pos.x += 0.005;
}

void entity_think_camera(entity_t *self){
    Vector3D target;
    Matrix4 perspective;
    int x,y;

    if(!self){
        slog("Tried to think NULL entity");
        return;
    }

    if(entity_keys[SDL_SCANCODE_W]){
        self->pos.y += 0.1;
    } else if(entity_keys[SDL_SCANCODE_S]){
        self->pos.y -= 0.1;
    }
    if(entity_keys[SDL_SCANCODE_A]){
        self->pos.x += 0.1;
    } else if(entity_keys[SDL_SCANCODE_D]){
        self->pos.x -= 0.1;
    }
    if(entity_keys[SDL_SCANCODE_E]){
        self->pos.z += 0.1;
    } else if(entity_keys[SDL_SCANCODE_Q]){
        self->pos.z -= 0.1;
    }


    SDL_GetMouseState(&x, &y);
    self->relative_rotation.x = ( camera_get_height() / (float) 2 - y ) / (float) 150;
    self->relative_rotation.y = ( camera_get_width() / (float) 2 - x ) / (float) 150;

    if(self->relative_rotation.x > CAMERA_MAX_ROTATION)
    {
        self->relative_rotation.x = CAMERA_MAX_ROTATION;
    } 
    else if (self->relative_rotation.x < -CAMERA_MAX_ROTATION)
    {
        self->relative_rotation.x = -CAMERA_MAX_ROTATION;
    }

    if(self->relative_rotation.y > CAMERA_MAX_ROTATION)
    {
        self->relative_rotation.y = CAMERA_MAX_ROTATION;
    }
    else if(self->relative_rotation.y < -CAMERA_MAX_ROTATION)
    {
        self->relative_rotation.y = -CAMERA_MAX_ROTATION;
    }

    //vector3d_add(self->rotation, self->relative_rotation, self->rotation);
    vector3d_copy(self->rotation, self->relative_rotation);

    target = vector3d(self->pos.x - CAMERA_DEFUALT_X,
                      self->pos.y - CAMERA_DEFUALT_Y,
                      self->pos.z - CAMERA_DEFUALT_Z);
    camera_set_pos(self->pos);
    camera_set_target(target);

    camera_get_perspective(perspective);

    gf3d_matrix_rotate(perspective, perspective, -self->rotation.x, AXIS_X);
    gf3d_matrix_rotate(perspective, perspective, -self->rotation.y, AXIS_Y);
    //gf3d_matrix_rotate(perspective, perspective, self->rotation.z, AXIS_Z);
    
    camera_set_perspective(perspective);

    
}

//THINK FUNCTIONS END

//TOUCH FUNCTIONS START

void entity_touch_destroy_other(entity_t *self, entity_t* other){
    if(!self  || !other){
        return;
    }
    if(other){
        if(other->Destroy){
            other->Destroy(other);
        }
    }
    slog("Destroyed other");
}

void entity_touch_stop_moving(entity_t *self, entity_t* other){
    if(!self || !other){
        return;
    }

    vector3d_set(self->velocity, 0,0,0);
    
}

//TOUCH FUNCTIONS END

void entity_scale_entity(entity_t *ent, float x, float y, float z){
    int i;
    float *shape, *scale;
    if(!ent){
        return;
    }
    ent->scale.x = x;
    ent->scale.y = y;
    ent->scale.z = z;

    scale = (float*) &ent->scale;
    shape = (float*) &ent->shape.shape;

    for(i = 0; i < 3; i++){
        shape[i] = shape[i]*scale[i]; 
    }
}

void entity_set_draw_ubo(entity_t *self){
    if(!self){
        slog("Tried to set position NULL entity");
        return;
    }

    //Set Position
    self->ubo.model[3][0] = self->pos.x;
    self->ubo.model[3][1] = self->pos.y;
    self->ubo.model[3][2] = self->pos.z;

    //Set Rotation
    gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->relative_rotation.x, AXIS_X);
    gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->relative_rotation.y, AXIS_Y);
    gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->relative_rotation.z, AXIS_Z);
    
    memset(&self->relative_rotation, 0, sizeof(Vector3D));

    //Set Scale
    self->ubo.model[0][0] *= self->scale.x;
    self->ubo.model[1][1] *= self->scale.y;
    self->ubo.model[2][2] *= self->scale.z;

    vector3d_set(self->scale, 1, 1, 1);

    camera_get_perspective(self->ubo.proj);
    camera_get_view(self->ubo.view);

}

void entity_generic_draw(entity_t *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer){
    if(!self){
        slog("Tried to draw NULL entity");
        return;
    }
    if(!self->model){
        //Assuming not error, invisible entities
        return;
    }
    if(!self->model->texture){
        return;
    }
    entity_set_draw_ubo(self);
    gf3d_vgraphics_update_ubo(&self->ubo, gf3d_vgraphics_uniform_buffer_get_index(self->model->ubo));
    //commandBuffer = gf3d_command_rendering_begin(bufferFrame);
    gf3d_model_draw(self->model, bufferFrame, commandBuffer);
    //gf3d_command_rendering_end(commandBuffer);
    //gf3d_vgraphics_render_end(bufferFrame);
}

void entity_generic_destroy(entity_t *self){
    if(!self){
        slog("Tried to destroy NULL entity");
        return;
    }
    self->in_use = 0;
    if(self->model){
        gf3d_model_free(self->model);
    }
    if(self->in_sim){
        physics_remove_body(self);
    }
    memset(self, 0, sizeof(entity_t));
    
}

void entity_system_think(Uint8 *keys){
    int i;
    entity_keys = keys;
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Think){
            entity_list[i].Think(&entity_list[i]);
        }
    }
}

void entity_system_draw(Uint32 bufferFrame, VkCommandBuffer commandBuffer){
    int i;
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].model){
            entity_generic_draw(&entity_list[i], bufferFrame, commandBuffer);
        }
    }
}

void entity_system_shutdown(){
    int i;
    slog("cleaning up entities");
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Destroy){
            entity_list[i].Destroy(&entity_list[i]);
        }
    }
    free(entity_list);
}


