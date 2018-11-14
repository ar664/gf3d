#include "simple_logger.h"
#include "simple_json.h"
#include "tile.h"

#define TILE_MAX_X  25
#define TILE_MAX_Y  25
#define TILE_OFFSET 2
#define TILE_DEPTH  -5


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

tile_t *tile_get_free_tile(){
    int i;
    for(i = 0; i < TILE_MAX_X*TILE_MAX_Y; i++){
        if(!tile_list[i].ent){
            return &tile_list[i];
        }
    }
    return NULL;
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

    tile_list[pos].building->relative_rotation.x = 45;
    tile_list[pos].building->pos.x = x*TILE_OFFSET;
    tile_list[pos].building->pos.y = TILE_DEPTH;
    tile_list[pos].building->pos.z = y*TILE_OFFSET;

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
        if(tile_list[i].ent){
            tile_list[i].ent->Destroy(tile_list[i].ent);
        }

        memset(&tile_list[i], 0, sizeof(tile_t));
        
    }
}