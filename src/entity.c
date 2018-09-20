#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "simple_logger.h"

entity_t* entity_list;


void entity_init(){
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
    atexit(entity_shutdown);
}

entity_t *entity_new(){
    int i;
    for(i = 0; i < ENTITY_MAX;i++){
        if(!entity_list[i].in_use){
            entity_list[i].in_use = 1;
            entity_list[i].Destroy = entity_generic_destroy;
        }
    }
    return NULL;
}

void entity_generic_destroy(entity_t *self){
    if(!self){
        slog("Tried to destroy NULL entity");
        return;
    }
    self->in_use = 0;
}

void entity_update(){
    int i;
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Update){
            entity_list[i].Update(&entity_list[i]);
        }
    }
}

void entity_shutdown(){
    int i;
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Destroy){
            entity_list[i].Destroy(&entity_list[i]);
        }
    }
    free(entity_list);
}


