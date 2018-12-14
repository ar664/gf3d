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
    SDL_Surface *surf, *fillSurf;
    SDL_Color bg;
    SDL_Rect texRect;
    char *filename;
    Uint32 rmask, gmask, bmask, amask;

    tex = malloc(sizeof(text_t));
    if(!text)
    {
        slog("unable to malloc text");
        return NULL;
    }

    filename = malloc(sizeof(char)*(strlen(text)+10));
    strcpy(filename, text);
    strcpy(&filename[strlen(text)+1], ".png");

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

    texRect.h = surf->h;
    texRect.w = surf->w;
    texRect.x = 0;
    texRect.y = 0;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    fillSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 32, rmask, gmask, bmask, amask);
    if(!fillSurf)
    {
        slog("failure to create fillSurf");
        return NULL;
    }
    
    //SDL_SetSurfaceAlphaMod(fillSurf, 255);
    SDL_FillRect(fillSurf, NULL, 0);
    SDL_BlitSurface(surf, &texRect, fillSurf, &rect);
    //memcpy(fillSurf, surf, sizeof(Uint32)*(surf->w*surf->h));

    tex->texture = gf3d_texture_from_surface(fillSurf);
    if(!tex->texture)
    {
        slog("Unable to create texture from surface!");
        return NULL;
    }
    IMG_SavePNG(fillSurf, filename);
    
    SDL_free(surf);
    SDL_free(fillSurf);
    free(filename);

    return tex;

}

void text_draw(text_t *text, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
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