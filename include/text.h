#ifndef __TEXT_H
#define __TEXT_H

#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "gf3d_texture.h"

typedef struct text_s text_t;

struct text_s
{
    SDL_Rect        window;
    Texture        *texture;
    SDL_Color       color;

    VkDevice        device;
    VkImageLayout   imageLayout;

    TTF_Font       *font;

    int             fontSize;
    char           *text;

};


/**
 * @brief Initialize TTF library
 * 
 */
void text_system_init(char *font, int size, VkDevice device);


text_t *text_load(char *text, SDL_Rect rect, SDL_Color color);
void text_update(text_t *text, char *words, SDL_Rect rect, SDL_Color color);
void text_draw(text_t *text,Uint32 bufferFrame, VkCommandBuffer commandBuffer);
void text_free(text_t *text);

#endif