#include <SDL.h>            

#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_camera.h"
#include "gf3d_object.h"
#include "gf3d_sprite.h"

int main(int argc,char *argv[])
{
    int done = 0;
    const Uint8 * keys;
    Object *sample_object;
    Sprite *sample_sprite;
    
    init_logger("gf3d.log");
    slog("gf3d begin");
    if( gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        1                       //validation
    ) != 0){
        slog("Fail to initialize graphics, exiting...");
        return -1;
    }

    gf3d_object_init();
    gf3d_sprite_init();
    sample_object = gf3d_object_load("models/chalet.obj");
    sample_sprite = gf3d_sprite_load("textures/sample.png", 1024,1024);
    
    // main game loop
    while(!done)
    {
        gf3d_vgraphics_clear();
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here
        
        
        gf3d_vgraphics_render();
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
