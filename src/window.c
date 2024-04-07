#include "window.h"

#include <math.h>

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

window_t *window_create(int width, int height, char *title)
{
	window_t *window = malloc(sizeof(window_t));
	window->title = malloc(strlen(title) + 1);
	if (window->title == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	}
	strcpy(window->title, title);
	window->window =
		SDL_CreateWindow(window->title, SDL_WINDOWPOS_CENTERED,
				 SDL_WINDOWPOS_CENTERED, width, height,
				 SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	if (window->window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		exit(1);
	}
	window->renderer = SDL_CreateRenderer(
		window->window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (window->renderer == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n",
			SDL_GetError());
		exit(1);
	}
	window->camera = camera_create();
	if (window->camera == NULL) {
		fprintf(stderr, "camera_create Error\n");
		exit(1);
	}
	if (window_surface_init(window)) {
		fprintf(stderr, "window_surface_init Error\n");
		exit(1);
	}
	window->keys = calloc(sizeof(bool), SDL_NUM_SCANCODES);
	window->mouse_left = false;
	window->controllable = true;
	window->mouse_sensitivity = 0.01;
	window->wheel_sensitivity = 0.7;
	window->key_sensitivity = 0.1;
	return window;
}

int window_destroy(window_t *window)
{
	free(window->title);
	window_surface_destroy(window);
	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyWindow(window->window);
	camera_destroy(window->camera);
	free(window->keys);
	free(window);
	return 0;
}

int window_clear(window_t *window)
{
	memset(window->sdl_surface->pixels, 0,
	       window->sdl_surface->h * window->sdl_surface->pitch);
	SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 0);
	SDL_RenderClear(window->renderer);
	return 0;
}

int window_surface_init(window_t *window)
{
	int win_w, win_h, rdr_w, rdr_h;
	SDL_GetWindowSize(window->window, &win_w, &win_h);
	SDL_GetRendererOutputSize(window->renderer, &rdr_w, &rdr_h);
	window->sdl_surface = SDL_CreateRGBSurface(
		0, rdr_w, rdr_h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
	if (window->sdl_surface == NULL) {
		fprintf(stderr, "SDL_CreateRGBSurfaceWithFormat Error: %s\n",
			SDL_GetError());
		return 1;
	}

	window->cr_surface = cairo_image_surface_create_for_data(
		window->sdl_surface->pixels, CAIRO_FORMAT_ARGB32,
		window->sdl_surface->w, window->sdl_surface->h,
		window->sdl_surface->pitch);
	if (window->cr_surface == NULL) {
		fprintf(stderr, "cairo_image_surface_create_for_data Error\n");
		return 1;
	}

	window->cr = cairo_create(window->cr_surface);
	if (window->cr == NULL) {
		fprintf(stderr, "cairo_create Error\n");
		return 1;
	}

	camera_viewport_set(window->camera, rdr_w, rdr_h);
	return 0;
}

int window_surface_destroy(window_t *window)
{
	cairo_destroy(window->cr);
	cairo_surface_destroy(window->cr_surface);
	SDL_FreeSurface(window->sdl_surface);
	return 0;
}

int window_render(window_t *window, draw_list_t *draw_list)
{
	return draw_list_render(draw_list, window->cr, window->camera);
}

int window_render_at(window_t *window, draw_list_t *draw_list, double x,
		     double y, double z, double rx, double ry, double rz)
{
	// write object position and rotation and restore later
	double tx, ty, tz, trx, try, trz;
	camera_object_position_get(window->camera, &tx, &ty, &tz);
	camera_object_rotation_get(window->camera, &trx, &try, &trz);
	camera_object_position_set(window->camera, x, y, z);
	camera_object_rotation_set(window->camera, rx, ry, rz);
	int ret = draw_list_render(draw_list, window->cr, window->camera);
	camera_object_position_set(window->camera, tx, ty, tz);
	camera_object_rotation_set(window->camera, trx, try, trz);
	return ret;
}

int window_render_end(window_t *window)
{
	SDL_Texture *texture = SDL_CreateTextureFromSurface(
		window->renderer, window->sdl_surface);
	if (texture == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n",
			SDL_GetError());
		return 1;
	}
	SDL_RenderCopy(window->renderer, texture, NULL, NULL);
	SDL_RenderPresent(window->renderer);
	SDL_DestroyTexture(texture);
	return 0;
}

int window_handle_events(window_t *window, event_list_t *event_list)
{
	SDL_Event event;
	int num_events = event_list_length(event_list);
	bool focus = SDL_GetWindowFlags(window->window) &
		     SDL_WINDOW_INPUT_FOCUS;
	for (int i = 0; i < num_events; i++) {
		event_list_get(event_list, i, &event);
		switch (event.type) {
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				window_surface_destroy(window);
				window_surface_init(window);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				return 1;
				break;
			}
			break;
		case SDL_KEYDOWN:
			if (focus) {
				window->keys[event.key.keysym.scancode] = true;
			}
			break;
		case SDL_KEYUP:
			if (focus) {
				window->keys[event.key.keysym.scancode] = false;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT && focus) {
				window->mouse_left = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT && focus) {
				window->mouse_left = false;
			}
			break;
		case SDL_MOUSEMOTION:
			if (window->mouse_left && focus &&
			    window->controllable) {
				double xrel = event.motion.xrel;
				double yrel = event.motion.yrel;
				double drz = -xrel * window->mouse_sensitivity;
				double dry = -yrel * window->mouse_sensitivity;
				camera_rotation_add(window->camera, 0.0, dry,
						    -drz);
			}
			break;
		case SDL_MOUSEWHEEL:
			if (focus && window->controllable) {
				double dz = event.wheel.y *
					    window->wheel_sensitivity;
				camera_distance_add(window->camera, dz);
			}
			break;
		}
	}
	for (keymapping_t *key = default_spherical_keymap; key->key != 0;
	     key++) {
		if (window->keys[key->key]) {
			window_do_key_action(window, key->action);
		}
	}
	return 0;
}

int window_do_key_action(window_t *window, key_action_t action)
{
	double k = window->key_sensitivity;
	double m = window->key_sensitivity;
	if (!window->controllable)
		return 0;
	switch (action) {
	case KEY_ACTION_X_INC:
		camera_move(window->camera, k, 0.0, 0.0);
		break;
	case KEY_ACTION_X_DEC:
		camera_move(window->camera, -k, 0.0, 0.0);
		break;
	case KEY_ACTION_Y_INC:
		camera_move(window->camera, 0.0, k, 0.0);
		break;
	case KEY_ACTION_Y_DEC:
		camera_move(window->camera, 0.0, -k, 0.0);
		break;
	case KEY_ACTION_Z_INC:
		camera_move(window->camera, 0.0, 0.0, k);
		break;
	case KEY_ACTION_Z_DEC:
		camera_move(window->camera, 0.0, 0.0, -k);
		break;
	case KEY_ACTION_RZ_INC:
		camera_rotation_add(window->camera, 0.0, 0.0, m);
		break;
	case KEY_ACTION_RZ_DEC:
		camera_rotation_add(window->camera, 0.0, 0.0, -m);
		break;
	case KEY_ACTION_RY_INC:
		camera_rotation_add(window->camera, 0.0, m, 0.0);
		break;
	case KEY_ACTION_RY_DEC:
		camera_rotation_add(window->camera, 0.0, -m, 0.0);
		break;
	case KEY_ACTION_DISTANCE_INC:
		camera_distance_add(window->camera, k);
		break;
	case KEY_ACTION_DISTANCE_DEC:
		camera_distance_add(window->camera, -k);
		break;
	default:
		break;
	}
	return 0;
}

camera_t *window_camera_get(window_t *window)
{
	return window->camera;
}

int window_save_png(window_t *window, char *filename)
{
	SDL_Surface *surface = SDL_CreateRGBSurface(0, window->sdl_surface->w,
						    window->sdl_surface->h, 32,
						    0x00ff0000, 0x0000ff00,
						    0x000000ff, 0);
	SDL_RenderReadPixels(window->renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
			     surface->pixels, surface->pitch);
	SDL_SaveBMP(surface, filename);
	SDL_FreeSurface(surface);
	return 0;
}

int window_save(window_t *window, uint8_t *buffer, size_t size)
{
	if ((int)size < window->sdl_surface->w * window->sdl_surface->h * 3)
		return 1;
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
		buffer, window->sdl_surface->w, window->sdl_surface->h, 24,
		window->sdl_surface->w * 3, 0x00ff0000, 0x0000ff00, 0x000000ff,
		0);
	SDL_RenderReadPixels(window->renderer, NULL, SDL_PIXELFORMAT_RGB24,
			     surface->pixels, surface->pitch);
	SDL_FreeSurface(surface);
	return 0;
}

bool window_controllable_get(window_t *window)
{
	return window->controllable;
}

int window_controllable_set(window_t *window, bool controllable)
{
	window->controllable = controllable;
	return 0;
}

double window_mouse_sensitivity_get(window_t *window)
{
	return window->mouse_sensitivity;
}

int window_mouse_sensitivity_set(window_t *window, double sensitivity)
{
	window->mouse_sensitivity = sensitivity;
	return 0;
}

double window_wheel_sensitivity_get(window_t *window)
{
	return window->wheel_sensitivity;
}

int window_wheel_sensitivity_set(window_t *window, double sensitivity)
{
	window->wheel_sensitivity = sensitivity;
	return 0;
}

double window_key_sensitivity_get(window_t *window)
{
	return window->key_sensitivity;
}

int window_key_sensitivity_set(window_t *window, double sensitivity)
{
	window->key_sensitivity = sensitivity;
	return 0;
}