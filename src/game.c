#include <SDL.h>            
#include <time.h>

#include "simple_logger.h"
#include "camera.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_camera.h"
#include "gf3d_vector.h"
#include "gf3d_texture.h"
#include "gf3d_audio.h"
#include "entity.h"
#include "tile.h"
#include "physics.h"
#include "text.h"
#include "game.h"

typedef struct{
    int         points;
    int         resource;
    int         waveCount;
    int         wave;
    entity_t  **enemy_list;
    
}gM;

gM gameManager = {0};

void game_points_add(int points){
    gameManager.points += points;
}

void game_resource_add(int resource){
    gameManager.resource += resource;
}

int main(int argc,char *argv[])
{
    int done = 0, i;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    entity_t *camEntity, *entity2, *entity3, *uiEntity;
    Mix_Music *music;
    SDL_Rect uiRect;
    SDL_Color uiColor;
    //Vector2D samplePos;
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    srand(time(NULL));
    camera_init(1280,720);
    
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1280,                   //screen width
        720,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        1                       //validation
    );
    gf3d_audio_init(AUDIO_DEFAULT_MAX_SOUNDS,
                    AUDIO_DEFAULT_MAX_CHANNELS,
                    AUDIO_DEFAULT_CHANNEL_GROUPS,
                    1,
                    1,
                    1);
    entity_system_init();
    tile_system_init();
    physics_system_init();
    
    text_system_init("ttf/runescape_font.ttf", 14);

    // main game loop
    slog("gf3d main loop begin");
    slog("vgraphics.ubo count = %u",gf3d_vgraphics_get_ubo_count());

    //entity1 = entity_load("cube");
    music = Mix_LoadMUS("music/better.ogg");
    if(!music){
        slog("Music Error: %s", Mix_GetError());
    } else{
        Mix_PlayMusic(music, -1);
    }
    
    uiRect.x = 0;
    uiRect.y = 0;
    uiRect.h = 200;
    uiRect.w = 200;

    uiColor.a = 255;
    uiColor.r = 255;
    uiColor.g = 0;
    uiColor.b = 255;


    camEntity = entity_load("camera");
    if(!camEntity){
        slog("Failed to load camEntity");
        return -1;
    }
    camEntity->Think = entity_think_camera;
    camEntity->pos = vector3d(CAMERA_DEFUALT_X,
                                CAMERA_DEFUALT_Y,
                                CAMERA_DEFUALT_Z);
    camEntity->scale.y = 10;

    uiEntity = entity_load("camera");
    uiEntity->pos = vector3d(CAMERA_DEFUALT_X +1,
                             CAMERA_DEFUALT_Y,
                             CAMERA_DEFUALT_Z);
    uiEntity->extra_data = text_load("Hello world", uiRect,uiColor);
    uiEntity->model->texture = ((text_t *)uiEntity->extra_data)->texture;
    gf3d_model_update_descriptor_sets(uiEntity->model);
    //uiEntity->Think = entity_think_ui;

    tile_load(0, 0, "resource");
    tile_load(1, 1, "unitFlying");
    for(i = 0; i < 3; i++)
    {
        tile_load(rand()%TILE_MAX_X, rand()%TILE_MAX_Y, "resource");
    }

    entity2 = entity_load("cube");
    entity2->pos = vector3d(0,0,-5);
    entity2->relative_rotation.x = 90;
    entity_scale_entity(entity2, 50, 50, 1);
    physics_add_body(entity2);


    entity3 = entity_load("cube");
    entity3->Think = entity_think_test_path;
    entity3->Touch = NULL;
    vector3d_copy(entity3->pos,tile_get_real_position(0,10)); 

    
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here
        camera_update();
        physics_system_update();
        entity_system_think((Uint8*)keys);

        //gf3d_vgraphics_move_model(vector3d(0,5,10));
        //entity2->pos.x += 0.005;
        //gf3d_vgraphics_rotate_camera(0.001);
    
        // configure render command for graphics command pool
        // for each mesh, get a command and configure it from the pool
        bufferFrame = gf3d_vgraphics_render_begin();
        commandBuffer = gf3d_command_rendering_begin(bufferFrame);

            entity_system_draw(bufferFrame, commandBuffer);
            //gf3d_model_draw(model2,bufferFrame,commandBuffer);
            
        gf3d_command_rendering_end(commandBuffer);
        gf3d_vgraphics_render_end(bufferFrame);

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
