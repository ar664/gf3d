#include <stdlib.h>
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
        entity_list[i].Update = NULL;
        entity_list[i].Destroy = NULL;
    }
    atexit(entity_destroy);
}

void entity_update(){
    int i;
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Update){
            entity_list[i].Update(&entity_list[i]);
        }
    }
}

void entity_destroy(){
    int i;
    for(i = 0; i < ENTITY_MAX; i++){
        if(entity_list[i].Destroy){
            entity_list[i].Destroy(&entity_list[i]);
        }
    }
    free(entity_list);
}


