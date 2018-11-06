#ifndef __TILE_H
#define __TILE_H

#include "gf3d_types.h"
#include "gf3d_vector.h"
#include "entity.h"

typedef struct tile_s tile_t;


struct tile_s {
    Uint8       buildable;      /**< Check whether a building can be built onto this tile */
    Uint8       x;              /**< X position of tile */
    Uint8       y;              /**< Y position of tile */
    Uint8       width;          /**< Width of building taking tile */
    Uint8       height;         /**< Height of building taking tile */
    entity_t   *building;       /**< Building tied to this tile */
    entity_t   *ent;            /**< Entity tied to this tile */
    char       *name;           /**< Name of the tile */

}

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
 * @param x         
 * @param y         Y position of tile
 * @param tileName  The name of the tile
 */
void tile_load(Uint8 x, Uint8 y, char *tileName);



#endif