#ifndef __PATH_H
#define __PATH_H

#include "gf3d_types.h"
#include "gf3d_vector.h"

// This path uses the tiling system to check whether a space is occupied by a building

typedef struct path_s path_t;

struct path_s {
    Point2D    *path;       /**< A set of points that describes the path */
    Uint32      count;      /**< The amount of steps needed for the path */
    Uint32      current;    /**< Hiding entity test position data here */
};

/**
 * @brief Get a path from start to end
 * 
 * @param start     The starting position
 * @param end       The ending position       
 * @return path_t*  An allocated path that is points to a path.
 * 
 */
path_t path_get_path(Point2D start, Point2D end);

/**
 * @brief Get a step from the path algorithm (Best w/ Dijkstra's)
 * 
 * @param start     The starting position
 * @param end       The ending position
 * @return Vector2D The next point in the step function.
 */
Point2D path_get_step(Point2D start, Point2D end);

/**
 * @brief Free the path allocated
 * 
 * @param path 
 */
void path_free(path_t *path);

#endif