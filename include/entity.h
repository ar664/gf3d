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
 * @param Touch         Things that happen when entity touches another entity
 */
struct entity_s{

    int update_next;
    int in_use;

    entity_t *parent;
    void (*Update)(entity_t *self);
    void (*Destroy)(entity_t *self);
    void (*Touch)(entity_t *self, entity_t *other); 
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
 * @brief Create a new entity, uses entity list
 * 
 * @return entity_t* that has been allocated
 */
entity_t *entity_new();

/**
 * @brief Updates the entities every frame
 * 
 */
void entity_update();

/**
 * @brief Destroys all entities before exit
 * 
 */
void entity_shutdown();

/**
 * @brief Destroys an entity on exit/during runtime
 * 
 */
void entity_generic_destroy(entity_t *self);

#endif