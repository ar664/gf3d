#include <SDL.h>            

#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_camera.h"
#include "gf3d_vector.h"
#include "gf3d_texture.h"
#include "entity.h"

int main(int argc,char *argv[])
{
    int done = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    entity_t *entity1, *entity2, *entity3;
    //Model *model;
    //Model *model2;
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    srand(time(NULL));
    entity_system_init();
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        1                       //validation
    );
    
    // main game loop
    slog("gf3d main loop begin");
    
    entity1 = entity_load("cube");
    slog("vgraphics.ubo count = %u",gf3d_vgraphics_get_ubo_count());
    entity2 = entity_load("cube");
    slog("vgraphics.ubo count = %u",gf3d_vgraphics_get_ubo_count());
    entity2->pos.x = 0;
    entity2->pos.y = -10;
    //model = gf3d_model_load("agumon");
    //model2 = gf3d_model_load("cube");
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        //update game things here
        entity_system_think((Uint8*)keys);

        //gf3d_vgraphics_move_model(vector3d(0,5,10));
        //entity2->pos.x += 0.005;
        if(entity2->pos.x > 1){
            entity2->Destroy(entity2);
            if(rand()%2){
                entity2 = entity_load("agumon");
            } else {
                entity2 = entity_load("cube");
            }
            
        }
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
