#ifndef __PHYSICS_H
#define __PHYSICS_H

#include "entity.h"

#define PHYSICS_BODY_MAX 1000
#define PHYSICS_MAX_VEL 10
#define PHYSICS_MAX_ACCEL 5
#define PHYSICS_MAX_RAYCAST 1000
#define PHYSICS_FRICTION 0.1

typedef struct{
    Vector3D pos;
    Vector3D direction;

}Ray;

typedef struct body_s body_t;

struct body_s {
    entity_t *ent;
};

/**
 * @brief Initialize the physics system with bodies and shapes
 * 
 */
void physics_system_init();

/**
 * @brief Currently n^2 physics collision checks
 * 
 */
void physics_system_update();

/**
 * @brief Add body to the physics simulation
 * 
 * @param ent 
 */
void physics_add_body(entity_t *ent);

/**
 * @brief Remove a body from the physics simulation
 * 
 * @param ent 
 */
void physics_remove_body(entity_t *ent);

/**
 * @brief Does a raycast to find an entity that intersects that ray
 * 
 * @param ray       position & direction
 * @oaran contact   The contact of the raycast
 * @return entity_t* entity that is in the physics simulation
 */
entity_t *physics_raycast(Ray ray, Vector3D *contact);

/**
 * @brief Does a ray cast with a point on screen
 * 
 * @param point     Point on screen (Usually where the mouse is)
 * @param contact   The contact of the raycast
 * @return entity_t* entity that is in the physics simulation
 */
entity_t *physics_raycast_point_on_screen(Point2D point, Vector3D *contact);

/**
 * @brief Find the closest entity to this entity of type layer
 * 
 * @param self      The entity being compared 
 * @param layer     The type of entity we're looking for
 * @return entity_t* The closest entity of type, if not NULL
 */
entity_t *physics_closest_entity(entity_t *self, int layer);

#endif