#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "simple_logger.h"

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

void entity_generic_draw(entity_t *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer){
    if(!self){
        slog("Tried to draw NULL entity");
        return;
    }
    gf3d_model_draw(self->model, bufferFrame, commandBuffer);
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


