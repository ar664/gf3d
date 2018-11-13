#ifndef __CAMERA_H
#define __CAMERA_H

#include "gf3d_types.h"
#include "gf3d_matrix.h"

typedef struct camera_s camera_t;

struct camera_s{
    Vector3D    pos;
    Vector3D    target;
    float       fov;
    float       near;
    float       far;
    int         width;
    int         height;
    Matrix4     view;
    Matrix4     perspective;

};

#define CAMERA_DEFUALT_X    2
#define CAMERA_DEFUALT_Y    20
#define CAMERA_DEFUALT_Z    2
#define CAMERA_DEFUALT_FOV  45*GF3D_DEGTORAD
#define CAMERA_DEFUALT_NEAR 0.1
#define CAMERA_DEFUALT_FAR  100

/**
 * @brief Initialize the camera variables to their default values
 * 
 */
void camera_init(int width, int height);

/**
 * @brief Updates the calculations for the view & perspective matrices.
 * 
 */
void camera_update();

/**
 * @brief Get the current view matrix of the camera.
 * 
 * @return Matrix4 A pointer to the view of the camera
 */
void camera_get_view(Matrix4 out);

/**
 * @brief Get the current perspective matrix of the camera.
 * 
 * @return Matrix4* A pointer to the perspective of the camera
 */
void camera_get_perspective(Matrix4 out);

/**
 * @brief Set the camera position
 * 
 * @param pos The position in (x,y,z) 
 */
void camera_set_pos(Vector3D pos);

/**
 * @brief Set the target that the camera is looking at
 * 
 * @param targ The target in (x,y,z) 
 */
void camera_set_target(Vector3D targ);

#endif