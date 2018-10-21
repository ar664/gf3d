#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "simple_logger.h"
#include "gf3d_vgraphics.h"

entity_t* entity_list;

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
        memset(&entity_list[i], 0, sizeof(struct entity_s));
        gf3d_matrix_identity(entity_list[i].ubo.model);
        gf3d_matrix_identity(entity_list[i].ubo.proj);
        gf3d_matrix_identity(entity_list[i].ubo.view);
        gf3d_matrix_view(
            entity_list[i].ubo.view,
            vector3d(2,20,2),
            vector3d(0,0,0),
            vector3d(0,0,1)
        );
        gf3d_matrix_perspective(
            entity_list[i].ubo.proj,
            45 * GF3D_DEGTORAD,
            1200/(float)700,
            0.1f,
            100
        );
        entity_list[i].ubo.proj[1][1] *= -1;
    }
    atexit(entity_system_shutdown);
}

entity_t *entity_new(){
    int i;
    for(i = 0; i < ENTITY_MAX;i++){
        if(!entity_list[i].in_use){
            entity_list[i].in_use = 1;
            entity_list[i].Destroy = (void*) entity_generic_destroy;
            return &entity_list[i];
        }
    }
    return NULL;
}

entity_t *entity_load(char *model){
    entity_t *ent;
    ent = entity_new();
    ent->model = gf3d_model_load(model);
    ent->Think = entity_generic_think;
    return ent;
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
}

void entity_set_draw_position(entity_t *self){
    if(!self){
        slog("Tried to set position NULL entity");
        return;
    }
    self->ubo.model[3][0] = self->pos.x;
    self->ubo.model[3][1] = self->pos.y;
    self->ubo.model[3][2] = self->pos.z;
    
}

void entity_generic_draw(entity_t *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer){
    if(!self){
        slog("Tried to draw NULL entity");
        return;
    }
    entity_set_draw_position(self);
    //commandBuffer = gf3d_command_rendering_begin(bufferFrame);
    gf3d_model_draw(self->model, bufferFrame, commandBuffer);
    //gf3d_command_rendering_end(commandBuffer);
    gf3d_vgraphics_update_ubo(&self->ubo, bufferFrame);
    //gf3d_vgraphics_render_end(bufferFrame);
}

void entity_generic_destroy(entity_t *self){
    if(!self){
        slog("Tried to destroy NULL entity");
        return;
    }
    self->in_use = 0;
}

void entity_system_think(){
    int i;
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


