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


int main(int argc,char *argv[])
{
    int done = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    entity_t *camEntity, *entity2, *entity3, *entity4;
    Mix_Music *music;
    //Model *model;
    //Model *model2;
    
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

    // main game loop
    slog("gf3d main loop begin");
    slog("vgraphics.ubo count = %u",gf3d_vgraphics_get_ubo_count());

    //entity1 = entity_load("cube");
    music = Mix_LoadMUS("music/better.mp3");
    if(!music){
        slog("Music Error: %s", Mix_GetError());
    } else{
        Mix_PlayMusic(music, -1);
    }
    

    camEntity = entity_load("");
    if(!camEntity){
        slog("Failed to load camEntity");
    }
    camEntity->Think = entity_think_camera;
    camEntity->pos = vector3d(CAMERA_DEFUALT_X,
                                CAMERA_DEFUALT_Y,
                                CAMERA_DEFUALT_Z);
    
    tile_load(0, 0, "tile");
    //tile_load(0, 0, "tile");
    
    entity2 = entity_load("cube");
    entity2->pos = vector3d(0,0,-5);
    entity2->relative_rotation.x = 90;
    entity_scale_entity(entity2, 50, 50, 1);


    entity3 = entity_load("cube");
    entity3->Think = entity_think_rotate_self_x;
    entity3->Touch = NULL;
    entity3->pos.x = 5;
    entity3->pos.y = 5;
    entity3->pos.z = 0;
    physics_add_body(entity3);

    entity4 = entity_load("cube");
    entity4->velocity.y = 0.001;
    entity4->pos.x = 5;
    entity4->pos.y = -5;
    entity4->pos.z = 0;
    entity4->Touch = NULL;//entity_touch_destroy_other;
    physics_add_body(entity4);
    
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
