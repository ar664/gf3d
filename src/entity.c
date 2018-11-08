#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "simple_logger.h"
#include "SDL.h"
#include "gf3d_vgraphics.h"

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
    ent->model = gf3d_model_load(model);
    ent->Think = NULL;//&entity_generic_think;
    gf3d_matrix_identity(ent->ubo.model);
    gf3d_matrix_identity(ent->ubo.proj);
    gf3d_matrix_identity(ent->ubo.view);
    gf3d_matrix_view(
        ent->ubo.view,
        vector3d(2,20,2),
        vector3d(0,0,0),
        vector3d(0,0,1)
    );
    gf3d_matrix_perspective(
        ent->ubo.proj,
        45 * GF3D_DEGTORAD,
        1200/(float)700,
        0.1f,
        100
    );
    ent->ubo.proj[1][1] *= -1;
    ent->scale = 1;
    return ent;
}

void entity_rotate_self_x(entity_t *self){
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

void entity_generic_think(entity_t *self){
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

    if(entity_keys[SDL_SCANCODE_E]){
        self->scale += 0.005;
    } else if (entity_keys[SDL_SCANCODE_Q]){
        self->scale -= 0.005;
    }

    //self->pos.x += 0.005;
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
    self->ubo.model[0][0] *= self->scale;
    self->ubo.model[1][1] *= self->scale;
    self->ubo.model[2][2] *= self->scale;
    //self->ubo.model[3][3] *= self->scale;

    self->scale=1;

}

void entity_generic_draw(entity_t *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer){
    if(!self){
        slog("Tried to draw NULL entity");
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
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Destroy){
            entity_list[i].Destroy(&entity_list[i]);
        }
    }
    free(entity_list);
}


