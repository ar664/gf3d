#ifndef __TILE_H
#define __TILE_H

#include "gf3d_types.h"
#include "gf3d_vector.h"
#include "entity.h"

#define TILE_TYPE_RESOURCE_STR  "resource"
#define TILE_TYPE_UNIT_STR      "unit"

#define TILE_MAX_X  10
#define TILE_MAX_Y  10
#define TILE_RESOURCE_TICK 600

typedef struct tile_s tile_t;

struct tile_s {
    Uint8       buildable;      /**< Check whether a building can be built onto this tile */
    int         x;              /**< X position of tile */
    int         y;              /**< Y position of tile */
    entity_t   *building;       /**< Building tied to this tile */
    char       *name;           /**< Name of the tile */
    int         resource_count; /**<Amount of resources left in building*/
    int         resource_gen;   /**<Amount of resources to mine each tick*/
    char       *unit;           /**<The unit this building can spawn*/
};

void tile_system_init();

/**
 * @brief Loads a map with preconfigured tiles from a json file.
 * 
 * @param mapName The name of the mape being loaded
 */
void tile_load_map(char *mapName);

/**
 * @brief Load a tile into the scene. Mostly Used for buildings.
 * 
 * @param x         X coord of tile
 * @param y         Y coord of tile
 * @param tileName  The name of the tile
 */
void tile_load(int x, int y, char *tileName);

/**
 * @brief Get the tile that is in x , y
 * 
 * @param x         X Coordinate of tile 
 * @param y         Y Coordinate of tile
 * @return tile_t* 
 */
tile_t *tile_get_tile(int x, int y);

/**
 * @brief Get the world position of the tile space located (x,y)
 * 
 * @param x         X coord of tile
 * @param y         Y coord of tile
 * @return Vector3D The (x,y,z) real world position of the tile
 */
Vector3D  tile_get_real_position(int x, int y);

/**
 * @brief Get the position in memory of the tile y + x*TILE_MAX_Y
 * 
 * @param x         X coord of tile
 * @param y         Y coord of tile 
 * @return int      The tile_list[>pos<] int
 */
int tile_get_memory_position(int x, int y);

#endif