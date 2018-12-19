#include "camera.h"

static camera_t Camera = {0};

void camera_init(int width, int height)
{
    Camera.far = CAMERA_DEFUALT_FAR;
    Camera.near = CAMERA_DEFUALT_NEAR;
    Camera.fov = CAMERA_DEFUALT_FOV;
    Camera.width = width;
    Camera.height = height;

    Camera.pos = vector3d(CAMERA_DEFUALT_X,
                          CAMERA_DEFUALT_Y,
                          CAMERA_DEFUALT_Z);
    Camera.target = vector3d(0,0,0);

    gf3d_matrix_identity(Camera.perspective);
    gf3d_matrix_identity(Camera.view);

    gf3d_matrix_view(Camera.view, 
                     Camera.pos, 
                     Camera.target, 
                     vector3d(0,0,1));
    
    gf3d_matrix_perspective(Camera.perspective, 
                            Camera.fov, 
                            width/(float)height, 
                            Camera.near, 
                            Camera.far);
    
    Camera.perspective[1][1] *= -1;
}

void camera_update(){
    gf3d_matrix_view(Camera.view, 
                     Camera.pos, 
                     Camera.target, 
                     vector3d(0,0,1));
    
    gf3d_matrix_perspective(Camera.perspective, 
                            Camera.fov, 
                            Camera.width/(float)Camera.height, 
                            Camera.near, 
                            Camera.far);

    Camera.perspective[1][1] *= -1;

}

void camera_get_view(Matrix4 out){
    if(!out) return;

    memcpy(out, Camera.view, sizeof(Matrix4));

}

void camera_set_view(Matrix4 in){
    if(!in) return;

    memcpy(Camera.view, in, sizeof(Matrix4));
}

void camera_get_perspective(Matrix4 out){
    if(!out) return;

    memcpy(out, Camera.perspective, sizeof(Matrix4));
}

void camera_set_perspective(Matrix4 in){
    if(!in) return;

    memcpy(Camera.perspective, in, sizeof(Matrix4));
}

void camera_set_pos(Vector3D pos){
    Camera.pos = pos;
}

void camera_set_target(Vector3D targ){
    Camera.target = targ;
}

int camera_get_width() {
    return Camera.width;
}
int camera_get_height(){
    return Camera.height;
}

Vector3D camera_get_position(){
    return Camera.pos;
}