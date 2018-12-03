#include "simple_logger.h"
#include "simple_json.h"
#include "tile.h"
#include "game.h"

#define TILE_MAX_X  10
#define TILE_MAX_Y  10
#define TILE_STEP   2.5
#define TILE_OFFSET 25  
#define TILE_DEPTH  -4
#define TILE_RESOURCE_TICK 600

tile_t tile_list[TILE_MAX_X*TILE_MAX_Y];

void tile_system_shutdown();

void tile_system_init(){
    int i,j;
    atexit(tile_system_shutdown);
    
    //How to loop through tiles
    for(i = 0; i < TILE_MAX_X; i++)
    {
        for(j = 0; j < TILE_MAX_Y; j++)
        {
            memset(&tile_list[j + i*TILE_MAX_Y], 0, sizeof(tile_t));
        }
    }
}

void tile_generate_resource(entity_t *self){
    tile_t *tile;
    int resource;

    tile = self->extra_data;
    if(!tile){
        return;
    }
    if(self->think_next > 0 ){
        self->think_next--;
        return;
    } else {
        self->think_next = TILE_RESOURCE_TICK;
    }
    
    if( tile->resource_gen > tile->resource_count) {
        resource = tile->resource_count;
        tile->resource_count = 0;
    } else {
        resource = tile->resource_gen; 
        tile->resource_count -= tile->resource_gen;
    } 
    //TODO: Update resource count text
    game_resource_add(resource);

}

void tile_touch_gather_minion(entity_t *self, entity_t *other){
    tile_t *tile;

    if(!self || !other){
        return;
    }

    tile = self->extra_data;
    if(!tile){
        return;
    }
    
    if(other->layer & LAYER_PLAYER)
    {
        if(other->Destroy){
            other->Destroy(other);
        }
        
        tile->resource_gen++;

        game_points_add(500);
    }
    

    
}

void tile_generate_unit(entity_t *self){

}

void *tile_get_think_func(char *tileName){
    return NULL;
}

Vector2D tile_get_dimensions(char *tileName){
    Vector2D a;
    a.x = 1;
    a.y = 1;
    return a;
}

void tile_load(int x, int y, char *tileName)
{
    Vector2D dimensions;
    int pos = y + x*TILE_MAX_Y;

    tile_list[pos].building = entity_load(tileName);

    if(!tile_list[pos].building){
        return;
    }
    tile_list[pos].building->relative_rotation.x = 45;
    tile_list[pos].building->pos.x = -x*TILE_STEP+TILE_OFFSET;
    tile_list[pos].building->pos.y = -y*TILE_STEP+TILE_OFFSET;
    tile_list[pos].building->pos.z = TILE_DEPTH;
    

    //The extra data used for think function
    tile_list[pos].building->extra_data = (void*) &tile_list[pos];

    dimensions = tile_get_dimensions(tileName);
    tile_list[pos].width = dimensions.x;
    tile_list[pos].height = dimensions.y;


}


void tile_system_shutdown(){
    int i;
    slog("tile system shutdown");
    for(i = 0; i < TILE_MAX_X*TILE_MAX_Y; i++)
    {
        if(tile_list[i].building){
            tile_list[i].building->Destroy(tile_list[i].building);
        }

        memset(&tile_list[i], 0, sizeof(tile_t));
        
    }
}