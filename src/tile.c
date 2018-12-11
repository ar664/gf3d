#include "simple_logger.h"
#include "simple_json.h"
#include <math.h>
#include "tile.h"
#include "physics.h"
#include "game.h"


#define TILE_OFFSET 25  
#define TILE_STEP   2.5
#define TILE_DEPTH  -4

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
    if(tile->resource_gen <= 0 )
    {
        return;
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
        slog("Minion added");
        game_points_add(500);
    }
    

    
}

void tile_generate_unit(entity_t *self){
    tile_t *tile;
    entity_t *ent;
    int i, j, x, y, pos;
    if(!self){
        return;
    }
    if(!self->extra_data){
        return;
    }
    if(self->think_next == -1)
    {
        return;
    }
    tile = (tile_t*) self->extra_data;

    //Check adjacent tiles for free spots
    for(i = 0; i < 2; i++){

        for(j = 0; i < 2; j++){
            x = tile->x + (i%2 ? 1 : -1);
            y = tile->y + (j%2 ? 1 : -1);

            if(x < 0 || x > TILE_MAX_X || y < 0 || y > TILE_MAX_Y)
            {
                break;
            }
            pos = y + x*TILE_MAX_Y;
            if(tile_list[pos].buildable){
                ent = entity_load(tile->unit);
                if(!ent)
                {
                    return;
                }
                vector3d_copy(ent->pos, tile_get_real_position(x, y));
                physics_add_body(ent);
                self->think_next = -1;
                return;
            }
        }
        
    }
    slog("No empty spots for unit");
    self->think_next = -1;


}

tile_t *tile_get_tile(int x, int y){
    if(x > TILE_MAX_X || 
       y > TILE_MAX_Y ||
       x < 0          ||
       y < 0)
    {
        slog("Tile out of range!");
        return NULL;
    }

    return &tile_list[tile_get_memory_position(x,y)];
}

int tile_get_memory_position(int x, int y){
    return (y + x*TILE_MAX_Y);
}

Point2D tile_get_closest_point(Vector3D pos){
    Point2D point;
    point.x = (int) -roundf( (pos.x-TILE_OFFSET)/TILE_STEP );
    point.y = (int) -roundf( (pos.y-TILE_OFFSET)/TILE_STEP );
    return point;
}

Vector3D tile_get_real_position(int x, int y){
    Vector3D a;
    a.x = -x*TILE_STEP+TILE_OFFSET;
    a.y = -y*TILE_STEP+TILE_OFFSET;
    a.z = TILE_DEPTH;
    return a;
}


void tile_load(int x, int y, char *tileName)
{
    char fn[128];
    char *value, *unit;
    SJson *json, *typeJ, *unitJ, *genJ, *countJ;

    int pos = tile_get_memory_position(x, y);
    tile_list[pos].building = entity_load(tileName);
    if(!tile_list[pos].building){
        return;
    }

    sprintf(fn, "json/%s.json", tileName);
    json = sj_load(fn);

    if(json) {

        typeJ = sj_object_get_value(json, "type");
        
        if(!typeJ)
        {
            slog("json no type, tile!");
            sj_free(json);
            return;
        }

        value = sj_get_string_value(typeJ);
        if(strcmp(TILE_TYPE_RESOURCE_STR, value) == 0) 
        {

            countJ = sj_object_get_value(json, "count");
            if(!countJ)
            {
                slog("json no count!");
                sj_free(json);
                return;
            }
            sj_get_integer_value(countJ, &tile_list[pos].resource_count);

            genJ = sj_object_get_value(json, "gen");
            if(!genJ)
            {
                slog("json no gen!");
                sj_free(json);
                return;
            }
            sj_get_integer_value(genJ, &tile_list[pos].resource_gen);

            tile_list[pos].building->Think = tile_generate_resource;
            tile_list[pos].building->Touch = tile_touch_gather_minion;
            tile_list[pos].building->think_next = TILE_RESOURCE_TICK;

        } else if (strcmp(TILE_TYPE_UNIT_STR, value) == 0) 
        {

            unitJ = sj_object_get_value(json, "unit");
            if(!unitJ)
            {
                slog("json no unit!");
                sj_free(json);
                return;
            }
            unit  = malloc(sizeof(char)*128);
            strcpy(unit, sj_get_string_value(unitJ));

            tile_list[pos].building->Think = tile_generate_unit;
            tile_list[pos].building->think_next = -1;
            tile_list[pos].unit = unit;

        } else {

            sj_free(json);
        }

    }

    vector3d_copy(tile_list[pos].building->pos, tile_get_real_position(x, y));
    

    //The extra data used for think function
    tile_list[pos].building->extra_data = (void*) &tile_list[pos];

    physics_add_body(tile_list[pos].building);

}


void tile_system_shutdown(){
    int i;
    slog("tile system shutdown");
    for(i = 0; i < TILE_MAX_X*TILE_MAX_Y; i++)
    {
        if(tile_list[i].building){
            tile_list[i].building->Destroy(tile_list[i].building);
            tile_list[i].building = NULL;
        }
        if(tile_list[i].unit){
            free(tile_list[i].unit);
            tile_list[i].unit = NULL;
        }

        memset(&tile_list[i], 0, sizeof(tile_t));
        
    }
}