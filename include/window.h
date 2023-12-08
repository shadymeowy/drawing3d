#ifndef WINDOW_H
#define WINDOW_H

#include "camera.h"
#include "eventlist.h"
#include "drawlist.h"
#include "keymapping.h"

struct window_s;
typedef struct window_s window_t;

window_t *window_create(int width, int height, char *title);
int window_destroy(window_t *window);
int window_surface_init(window_t *window);
int window_surface_destroy(window_t *window);
int window_clear(window_t *window);
int window_render(window_t *window, draw_list_t *draw_list);
int window_render_at(window_t *window, draw_list_t *draw_list, double x,
		     double y, double z, double rx, double ry, double rz);
int window_render_end(window_t *window);
int window_handle_events(window_t *window, event_list_t *event_list);
int window_do_key_action(window_t *window, key_action_t action);
camera_t *window_camera_get(window_t *window);
int window_save_png(window_t *window, char *filename);
int window_save(window_t *window, uint8_t *buffer, size_t size);
bool window_controllable_get(window_t *window);
int window_controllable_set(window_t *window, bool controllable);
double window_mouse_sensitivity_get(window_t *window);
int window_mouse_sensitivity_set(window_t *window, double sensitivity);
double window_wheel_sensitivity_get(window_t *window);
int window_wheel_sensitivity_set(window_t *window, double sensitivity);
double window_key_sensitivity_get(window_t *window);
int window_key_sensitivity_set(window_t *window, double sensitivity);

#endif