#ifndef __UNIT_H
#define __UNIT_H

#include "entity.h"

typedef struct unit_s unit_t;

typedef enum
{
    UNIT_TYPE_NORMAL,
    UNIT_TYPE_RANGE,
    UNIT_TYPE_FLYING
}UnitTypes;

struct unit_s
{
    int             attack;
    int             defense;
    int             health;
    int             cost;

    UnitTypes       type;

    char           *name;
    entity_t       *ent;

};

void unit_system_init();

/**
 * @brief Load a template for an entity does not include entity inside of it
 * 
 * @param name      The name of the unit to load json of it
 * @return unit_t*  The template of the unit
 * 
 * @NOTE This is a reference do not overwrite
 */
unit_t *unit_load_unit(char *name);

#endif