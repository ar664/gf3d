#ifndef _ENTITY_H
#define _ENTITY_H

#define ENTITY_MAX 1000

typedef struct entity_s entity_t;

/**
 * @breif The entity structure which controls all entitys
 * 
 * @param update_next   The next time this entity needs to update
 * @param Update        The update function for this entity
 * @param Destroy       Anything that needs to be destroyed before this entity dies
 */
struct entity_s{

    int update_next;
    void (*Update)(entity_t *self);
    void (*Destroy)(entity_t *self);
};

/**
 * @breif The global list of all entities. 
 */
extern entity_t *entity_list;


/**
 * @breif The function that allocates memory for the entity_list 
 */
void entity_init();

/**
 * @brief Updates the entities every frame
 * 
 */
void entity_update();

/**
 * @brief Destroys all entities before exit
 * 
 */
void entity_destroy();

#endif