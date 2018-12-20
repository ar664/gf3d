#include "unit.h"
#include "simple_logger.h"
#include "simple_json.h"

#define UNITS_MAX 50

unit_t Units[UNITS_MAX] = {0};

void unit_system_shutdown();

void unit_system_init()
{
    atexit(unit_system_shutdown);
}

unit_t *unit_load_unit(char *name)
{
    char fn[128];
    int i, value;
    char *allName;
    SJson *json, *statsJ;

    if(!name)
    {
        slog("Tried to load null unit");
        return NULL;
    }

    for(i = 0; i < UNITS_MAX; i++)
    {
        if(!Units[i].name)
        {
            break;
        }
        if(strcmp(Units[i].name, name))
        {
            return &Units[i];
        }
    }

    sprintf(fn, "json/%s.json", name);
    json = sj_load(fn);
    if(!json)
    {
        slog("Unit %s not found!", name);
        return NULL;
    }

    allName = malloc(sizeof(char)*(strlen(name)+1));
    if(!allName)
    {
        slog("Could not allocate name space for unit");
        return;
    }
    strcpy(allName, name);
    Units[i].name = allName;

    statsJ = sj_object_get_value(json, "attack");
    if(statsJ)
    {
        sj_get_integer_value(statsJ, &value);
        Units[i].attack = value;
    }
    statsJ = sj_object_get_value(json, "defense");
    if(statsJ)
    {
        sj_get_integer_value(statsJ, &value);
        Units[i].defense = value;
    }
    statsJ = sj_object_get_value(json, "health");
    if(statsJ)
    {
        sj_get_integer_value(statsJ, &value);
        Units[i].health = value;
    }
    statsJ = sj_object_get_value(json, "cost");
    if(statsJ)
    {
        sj_get_integer_value(statsJ, &value);
        Units[i].cost = value;
    }

    sj_free(json);

    return &Units[i];
}

void unit_system_shutdown()
{
    int i;
    for(i = 0; i < UNITS_MAX; i++)
    {
        if(Units[i].name)
        {
            free(Units[i].name);
        }
    }
    memset(Units, 0, sizeof(unit_t)*UNITS_MAX);
}