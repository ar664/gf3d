
#include "simple_logger.h"
#include "path.h"
#include "tile.h"



path_t path_get_path(Point2D start, Point2D end)
{
    int max, i, j;
    path_t path = {0};
    Point2D temp;

    if(end.x == start.x && end.y == start.y)
    {
        return path;
    }
    //max = distance(end, start)*2
    max = (int) (powf(end.y - start.y, 2) + powf(end.x - start.x, 2))*2;
    path.path = malloc(sizeof(Point2D)*max);
    temp = start;
    for(i = 0; i < max; i++)
    {
        path.path[i] = path_get_step(temp, end);
        temp = path.path[i];
        if(path.path[i].x == -1)
        {
            slog("Unable to get path!");
            return path;
        }
        if(path.path[i].x == end.x && path.path[i].y == end.y)
        {
            path.count = i+1;
            return path;
        }
    }

    path.count = max;
    return path;
}

//For now lazy best path
Point2D path_get_step(Point2D start, Point2D end)
{
    tile_t *chck;
    int i,ydir, x, y;
    Point2D direction;
    Point2D step = {-1};
    direction.x = end.x - start.x;
    direction.y = end.y - start.y;

    //1st direction
    ydir = abs(direction.y) > abs(direction.x);
    if( ydir )
    {
        x = start.x;
        y = start.y + (direction.y > 0 ? 1 : -1);
    } else {
        x = start.x + (direction.x > 0 ? 1 : -1);
        y = start.y;
    }
    chck = tile_get_tile(x, y);
    if(chck)
    {
        if(!chck->building)
        {
            step.x = x;
            step.y = y;
            return step;
        }
    }
    //2nd direction
    if(ydir)
    {
        x = start.x + (direction.x > 0 ? 1 : -1);
        y = start.y;
    } else
    {
        x = start.x;
        y = start.y + (direction.y > 0 ? 1 : -1);
    }
    chck = tile_get_tile(x, y);
    if(chck)
    {
        if(!chck->building)
        {
            step.x = x;
            step.y = y;
            return step;
        }
    }
    return step;

}

void path_free(path_t *path)
{
    if(path->path)
    {
        free(path->path);
        path->path = NULL;
    }
}