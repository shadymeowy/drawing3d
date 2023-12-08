#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

struct camera_s;
typedef struct camera_s camera_t; 

camera_t *camera_create();
int camera_destroy(camera_t *camera);
int camera_position_set(camera_t *camera, double x, double y, double z);
int camera_position_get(camera_t *camera, double *x, double *y, double *z);
int camera_position_add(camera_t *camera, double x, double y, double z);
int camera_move(camera_t *camera, double x, double y, double z);
int camera_rotation_set(camera_t *camera, double x, double y, double z);
int camera_rotation_get(camera_t *camera, double *x, double *y, double *z);
int camera_rotation_add(camera_t *camera, double x, double y, double z);
int camera_object_position_get(camera_t *camera, double *x, double *y, double *z);
int camera_object_position_set(camera_t *camera, double x, double y, double z);
int camera_object_position_add(camera_t *camera, double x, double y, double z);
int camera_object_rotation_get(camera_t *camera, double *x, double *y, double *z);
int camera_object_rotation_set(camera_t *camera, double x, double y, double z);
int camera_object_rotation_add(camera_t *camera, double x, double y, double z);
int camera_distance_set(camera_t *camera, double distance);
int camera_distance_get(camera_t *camera, double *distance);
int camera_distance_add(camera_t *camera, double distance);
int camera_perspective(camera_t *camera, double hfov, double vfov);
int camera_orthographic(camera_t *camera, double scale_x, double scale_y);
int camera_viewport_set(camera_t *camera, int width, int height);
int camera_viewport_get(camera_t *camera, int *width, int *height);
bool camera_preserve_ratio_get(camera_t *camera);
int camera_preserve_ratio_set(camera_t *camera, bool preserve_ratio);
int camera_projection_get(camera_t *camera, double m[16]);
int camera_projection_set(camera_t *camera, double m[16]);
bool camera_project(camera_t *camera, double *p1, double *p2);
int camera_update(camera_t *camera);

#endif