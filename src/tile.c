#include "tile.h"


#define TILE_MAX_X 25
#define TILE_MAX_Y 25

tile_t tile_list[TILE_MAX_X*TILE_MAX_Y];

void tile_system_shutdown();

void tile_system_init(){
    int i,j;
    atexit(tile_system_shutdown);
    
    //How to loop through tiles
    for(i = 0; i < TILE_MAX_X; i++)
    {
        for(j = 0; j < TILE_MAX_Y; i++)
        {
            memset(&tile_list[j + i*TILE_MAX_Y], 0, sizeof(tile_t));
        }
    }
}


void tile_system_shutdown(){

}