#include "gf3d_vgraphics.h"
#include "gf3d_texture.h"
#include "simple_logger.h"
#include "text.h"

typedef struct
{
    VkDevice    device;
}tM;

tM textManager  = {0};

text_t Text = {0};

void text_system_shutdown();
Uint32 rmask, gmask, bmask, amask;


void text_system_init(char *font, int size, VkDevice device)
{
    if(TTF_Init() == -1)
    {
        printf("TTF could not init: %s", TTF_GetError());
        return;
    }

    Text.fontSize = size;
    Text.font = TTF_OpenFont(font, Text.fontSize);

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

    textManager.device = device;

    atexit(TTF_Quit);
    atexit(text_system_shutdown);

}

text_t *text_load(char *words, SDL_Rect rect, SDL_Color color)
{
    text_t *text;
    SDL_Surface *surf, *fillSurf;
    SDL_Color bg;
    SDL_Rect textRect;
    char *filename;
    

    text = malloc(sizeof(text_t));
    if(!text)
    {
        slog("unable to malloc text");
        return NULL;
    }
    memset(text, 0, sizeof(text_t));

    filename = malloc(sizeof(char)*(strlen(words)+10));
    if(!filename)
    {
        slog("unable to create words file");
        free(text);
        return NULL;
    }
    strcpy(filename, words);
    strcpy(&filename[strlen(words)], ".png");

    bg.a = 150;
    bg.r = 100;
    bg.g = 0;
    bg.b = 50;

    text->font = Text.font;
    text->fontSize = Text.fontSize;

    text->window = rect;
    text->color = color;

    surf = TTF_RenderText_Shaded(text->font, words, color, bg);
    if(!surf)
    {
        slog("Unable to render words!");
        free(text);
        free(filename);
        return NULL;
    }

    textRect.h = surf->h;
    textRect.w = surf->w;
    textRect.x = 0;
    textRect.y = 0;

    

    fillSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 32, rmask, gmask, bmask, amask);
    if(!fillSurf)
    {
        slog("failure to create fillSurf");
        free(text);
        free(filename);
        SDL_free(surf);
        return NULL;
    }
    
    SDL_SetSurfaceAlphaMod(fillSurf, 0);
    SDL_FillRect(fillSurf, NULL, 0);
    SDL_BlitSurface(surf, &textRect, fillSurf, &rect);
    //memcpy(fillSurf, surf, sizeof(Uint32)*(surf->w*surf->h));

    text->texture = gf3d_texture_from_surface(fillSurf);
    if(!text->texture)
    {
        slog("Unable to create texture from surface!");
        free(text);
        free(filename);
        SDL_free(surf);
        SDL_free(fillSurf);
        return NULL;
    }
    IMG_SavePNG(fillSurf, filename);
    
    SDL_free(surf);
    SDL_free(fillSurf);
    free(filename);

    return text;

}

void text_update(text_t *text, char *words, SDL_Rect rect, SDL_Color color)
{
    SDL_Surface *surf, *fillSurf;
    SDL_Color bg;
    SDL_Rect textRect;
    void *data;
    VkDeviceSize imageSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    if(!text)
    {
        slog("Can't update null text");
        return;
    }

    bg.a = 150;
    bg.r = 100;
    bg.g = 0;
    bg.b = 50;

    surf = TTF_RenderText_Shaded(text->font, words, color, bg);
    if(!surf)
    {
        slog("Unable to render words!");
        free(text);
        return NULL;
    }

    textRect.h = surf->h;
    textRect.w = surf->w;
    textRect.x = 0;
    textRect.y = 0;
    

    fillSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, rect.w, rect.h, 32, rmask, gmask, bmask, amask);
    if(!fillSurf)
    {
        slog("failure to create fillSurf");
        free(text);
        SDL_free(surf);
        return NULL;
    }
    
    SDL_SetSurfaceAlphaMod(fillSurf, 0);
    SDL_FillRect(fillSurf, NULL, 0);
    SDL_BlitSurface(surf, &textRect, fillSurf, &rect);

    imageSize = fillSurf->w * fillSurf->h * 4;
    gf3d_vgraphics_create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    SDL_LockSurface(fillSurf);
        vkMapMemory(textManager.device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, fillSurf->pixels, imageSize);
        vkUnmapMemory(textManager.device, stagingBufferMemory);
    SDL_UnlockSurface(fillSurf); 

    gf3d_texture_copy_buffer_to_image(stagingBuffer, text->texture->textureImage, fillSurf->w, fillSurf->h);

    vkDestroyBuffer(textManager.device, stagingBuffer, NULL);
    vkFreeMemory(textManager.device, stagingBufferMemory, NULL);
    SDL_free(surf);
    SDL_free(fillSurf);

}

void text_draw(text_t *words, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
    VkDeviceSize offsets[] = {0};
    Pipeline *pipe;
    if (!words)
    {
        slog("cannot render a NULL words");
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