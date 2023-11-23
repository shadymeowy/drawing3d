#include "draw.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

struct camera_s {
	// viewport
	double width;
	double height;
	// extrinsic parameters
	double position[3];
	double rotation[3];
	double distance; // for spherical camera
	// intrinsic parameters, homogeneous
	double mi[16];
	// projection matrix
	double m[16];
	bool preserve_ratio;
	double ratio;
};

int matmul(double *a, double *b, double *c, int n, int m, int r);
int rotxyz(double *m, double x, double y, double z);
int translate(double *m, double x, double y, double z);
int eye(double *m, int n);
int permute(double *m, int i0, int i1, int i2, int i3);
int scale(double *m, double x, double y, double z, double w);

struct primitive_s {
	primitive_type_t type;
	size_t index;
	size_t length;
};

struct draw_list_s {
	size_t length;
	size_t length_saved;
	size_t capacity;
	size_t buffer_length;
	size_t buffer_length_saved;
	size_t buffer_capacity;
	primitive_t *primitives;
	double *buffer;
};

struct window_s {
	char *title;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Surface *sdl_surface;
	cairo_surface_t *cr_surface;
	cairo_t *cr;
	camera_t *camera;
	bool *keys;
	bool mouse_left;
	bool controllable;
	double mouse_sensitivity;
	double wheel_sensitivity;
	double key_sensitivity;
};

struct event_list_s {
	size_t length;
	size_t capacity;
	SDL_Event *events;
};