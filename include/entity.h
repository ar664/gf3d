#ifndef __ENTITY_H
#define __ENTITY_H

#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_vector.h"

#define ENTITY_MAX 1000

typedef struct entity_s entity_t;

enum Layer {
    LAYER_NULL      =   0,
    LAYER_PLAYER    =   1,
    LAYER_MISC      =   1<<2
};

/**
 * @breif The entity structure which controls all entitys
 * 
 * @param Think         The game logic update function
 * @param Update        The (Physics) update function for this entity
 * @param Destroy       Anything that needs to be destroyed before this entity dies
 * @param Touch         Things that happen when entity touches another entity
 */ 
struct entity_s{

    int                     think_next;         /**< Next time to update this entity */
    int                     in_use;             /**< Check whether this entity spot is in use */\
    
    float                   scale;              /**< Scale for model */

    entity_t               *parent;             /**< The parent of this entity (E.g. Rocket->Player) */

    Model                  *model;              /**< The object for the model */
    UniformBufferObject     ubo;                /**< The ubo for drawing */

    Vector3D                pos;                /**< Position for physics */
    Vector3D                velocity;           /**< Velocity for physics */
    Vector3D                acceleration;       /**<  Acceleration for physics*/

    Vector3D                axis;               /**< Axis of rotation */
    Vector3D                rotation;           /**< Rotation in degrees */
    Vector3D                relative_rotation;  /**< Change in rotation in a frame */

    void (*Think)(entity_t *self);
    void (*Update)(entity_t *self);
    void (*Destroy)(entity_t *self);
    void (*Touch)(entity_t *self, entity_t *other); 
};

/**
 * @breif The function that allocates memory for the entity_list 
 */
void entity_system_init();

/**
 * @brief Create a new entity, uses entity list
 * 
 * @return entity_t* That has been allocated
 */
entity_t *entity_new();

/**
 * @brief Load an entity with a model
 * 
 * @param   model     The model associated with this entity.
 * @return  entity_t* That has been allocated and set
 */
entity_t *entity_load(char *model);

/**
 * @brief Updates the entities every frame
 * 
 */
void entity_system_think(Uint8 *keys);

/**
 * @brief Draw all entities on the screen
 * 
 */
void entity_system_draw(Uint32 bufferFrame, VkCommandBuffer commandBuffer);

/**
 * @brief Destroys all entities before exit
 * 
 */
//void entity_system_shutdown();

void entity_generic_think(entity_t *self);

void entity_rotate_self_x(entity_t *self);

/**
 * @brief Destroys an entity on exit/during runtime
 * 
 */
void entity_generic_destroy(entity_t *self);

#endif