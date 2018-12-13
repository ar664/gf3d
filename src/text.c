#include "gf3d_vgraphics.h"
#include "simple_logger.h"
#include "text.h"


text_t Text = {0};

void text_system_shutdown();


void text_system_init(char *font, int size)
{
    if(TTF_Init() == -1)
    {
        printf("TTF could not init: %s", TTF_GetError());
        return;
    }
    //THIS IS HARD CODED
    Text.fontSize = size;
    Text.font = TTF_OpenFont(font, Text.fontSize);

    atexit(TTF_Quit);
    atexit(text_system_shutdown);

}

text_t *text_load(char *text, SDL_Rect rect, SDL_Color color)
{
    text_t *tex;
    SDL_Surface *surf;
    SDL_Color bg;

    tex = malloc(sizeof(text_t));
    if(!text)
    {
        slog("unable to malloc text");
        return NULL;
    }
    
    bg.a = 150;
    bg.r = 100;
    bg.g = 0;
    bg.b = 50;

    tex->font = Text.font;
    tex->fontSize = Text.fontSize;

    tex->window = rect;
    tex->color = color;

    surf = TTF_RenderText_Shaded(tex->font, text, color, bg);
    if(!surf)
    {
        slog("Unable to render text!");
        return NULL;
    }
    tex->texture = gf3d_texture_from_surface(surf);
    if(!tex->texture)
    {
        slog("Unable to create texture from surface!");
        return NULL;
    }


    return tex;

}

void text_draw(text_t *text, VkCommandBuffer commandBuffer)
{
    VkDeviceSize offsets[] = {0};
    Pipeline *pipe;
    if (!text)
    {
        slog("cannot render a NULL text");
        return;
    }
    
    

}

void text_free(text_t *text)
{
    if(!text)
    {
        return;
    }
    if(text->texture)
    {
        gf3d_texture_delete(text->texture);
    }
    if(text->text)
    {
        free(text->text);
    }
    memset(text, 0, sizeof(text_t));
}

void text_system_shutdown()
{

}