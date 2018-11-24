#ifndef __PHYSICS_H
#define __PHYSICS_H

#include "entity.h"

#define PHYSICS_BODY_MAX 1000
#define PHYSICS_MAX_VEL 10
#define PHYSICS_MAX_ACCEL 5
#define PHYSICS_FRICTION 0.1


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

#endif