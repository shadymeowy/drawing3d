#include "draw_priv.h"

window_t *window_create(int width, int height, char *title)
{
	window_t *window = malloc(sizeof(window_t));
	window->title = strdup(title);
	window->window =
		SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
				 SDL_WINDOWPOS_UNDEFINED, width, height,
				 SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI |
					 SDL_WINDOW_RESIZABLE);
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
	int cr_x_multiplier = rdr_w / win_w;
	int cr_y_multiplier = rdr_h / win_h;

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
	cairo_surface_set_device_scale(window->cr_surface, cr_x_multiplier,
				       cr_y_multiplier);

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
	window_clear(window);
	draw_list_render(draw_list, window->cr, window->camera);
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

int window_renders(window_t *window, size_t num, draw_list_t **draw_list)
{
	window_clear(window);
	for (int i = 0; i < num; i++) {
		draw_list_render(draw_list[i], window->cr, window->camera);
	}
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
						    drz);
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
	if (!window->controllable)
		return 0;
	switch (action) {
	case KEY_ACTION_X_INC:
		camera_move(window->camera, 0.1, 0.0, 0.0);
		break;
	case KEY_ACTION_X_DEC:
		camera_move(window->camera, -0.1, 0.0, 0.0);
		break;
	case KEY_ACTION_Y_INC:
		camera_move(window->camera, 0.0, 0.1, 0.0);
		break;
	case KEY_ACTION_Y_DEC:
		camera_move(window->camera, 0.0, -0.1, 0.0);
		break;
	case KEY_ACTION_Z_INC:
		camera_move(window->camera, 0.0, 0.0, 0.1);
		break;
	case KEY_ACTION_Z_DEC:
		camera_move(window->camera, 0.0, 0.0, -0.1);
		break;
	case KEY_ACTION_RZ_INC:
		camera_rotation_add(window->camera, 0.0, 0.0, M_PI / 180.0);
		break;
	case KEY_ACTION_RZ_DEC:
		camera_rotation_add(window->camera, 0.0, 0.0, -M_PI / 180.0);
		break;
	case KEY_ACTION_RY_INC:
		camera_rotation_add(window->camera, 0.0, M_PI / 180.0, 0.0);
		break;
	case KEY_ACTION_RY_DEC:
		camera_rotation_add(window->camera, 0.0, -M_PI / 180.0, 0.0);
		break;
	case KEY_ACTION_DISTANCE_INC:
		camera_distance_add(window->camera, 0.1);
		break;
	case KEY_ACTION_DISTANCE_DEC:
		camera_distance_add(window->camera, -0.1);
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
	if (size < window->sdl_surface->w * window->sdl_surface->h * 3)
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

event_list_t *event_list_create()
{
	event_list_t *event_list = malloc(sizeof(event_list_t));
	if (event_list == NULL)
		return NULL;
	event_list->length = 0;
	event_list->capacity = 4;
	event_list->events = calloc(sizeof(SDL_Event), event_list->capacity);
	if (event_list->events == NULL) {
		free(event_list);
		return NULL;
	}
	return event_list;
}

int event_list_destroy(event_list_t *event_list)
{
	free(event_list->events);
	free(event_list);
	return 0;
}

int event_list_reset(event_list_t *event_list)
{
	event_list->length = 0;
	return 0;
}

int event_list_append(event_list_t *event_list, SDL_Event *event)
{
	if (event_list->length == event_list->capacity) {
		event_list->capacity *= 2;
		event_list->events =
			realloc(event_list->events,
				sizeof(SDL_Event) * event_list->capacity);
	}
	event_list->events[event_list->length] = *event;
	event_list->length++;
	return 0;
}

int event_list_length(event_list_t *event_list)
{
	return event_list->length;
}

int event_list_get(event_list_t *event_list, int index, SDL_Event *event)
{
	if (index < 0 || index >= event_list->length)
		return 1;
	*event = event_list->events[index];
	return 0;
}

int event_list_poll(event_list_t *event_list)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		event_list_append(event_list, &event);
		if (event.type == SDL_QUIT)
			return 1;
	}
	return 0;
}

draw_list_t *draw_list_create()
{
	draw_list_t *draw_list = malloc(sizeof(draw_list_t));
	draw_list->length = 0;
	draw_list->length_saved = 0;
	draw_list->capacity = 4;
	draw_list->primitives =
		malloc(sizeof(primitive_t) * draw_list->capacity);
	draw_list->buffer_length = 0;
	draw_list->buffer_length_saved = 0;
	draw_list->buffer_capacity = 4;
	draw_list->buffer = malloc(sizeof(double) * draw_list->buffer_capacity);
	return draw_list;
}

int draw_list_destroy(draw_list_t *draw_list)
{
	free(draw_list->primitives);
	free(draw_list->buffer);
	free(draw_list);
	return 0;
}

int draw_list_save(draw_list_t *draw_list)
{
	draw_list->length_saved = draw_list->length;
	draw_list->buffer_length_saved = draw_list->buffer_length;
	return 0;
}

int draw_list_load(draw_list_t *draw_list)
{
	draw_list->length = draw_list->length_saved;
	draw_list->buffer_length = draw_list->buffer_length_saved;
	return 0;
}

int draw_list_empty(draw_list_t *draw_list)
{
	draw_list->length = 0;
	draw_list->buffer_length = 0;
	draw_list->length_saved = 0;
	draw_list->buffer_length_saved = 0;
	return 0;
}

int draw_list_buffer_allocate(draw_list_t *draw_list, size_t num)
{
	if (draw_list->buffer_length + num >= draw_list->buffer_capacity) {
		while (draw_list->buffer_length + num >=
		       draw_list->buffer_capacity) {
			draw_list->buffer_capacity *= 2;
		}
		draw_list->buffer =
			realloc(draw_list->buffer,
				sizeof(double) * draw_list->buffer_capacity);
	}
	return 0;
}

int draw_list_buffer_copy(draw_list_t *draw_list, size_t num, double *src)
{
	draw_list_buffer_allocate(draw_list, num);
	memcpy(draw_list->buffer + draw_list->buffer_length, src,
	       sizeof(double) * num);
	draw_list->buffer_length += num;
	return 0;
}

int draw_list_append(draw_list_t *draw_list, primitive_type_t type, size_t num)
{
	if (draw_list->length == draw_list->capacity) {
		draw_list->capacity *= 2;
		draw_list->primitives =
			realloc(draw_list->primitives,
				sizeof(primitive_t) * draw_list->capacity);
	}
	draw_list->primitives[draw_list->length].type = type;
	draw_list->primitives[draw_list->length].index =
		draw_list->buffer_length - num;
	draw_list->primitives[draw_list->length].length = num;
	draw_list->length++;
	return 0;
}

int draw_list_points(draw_list_t *draw_list, int num_points, double *points)
{
	if (num_points < 1)
		return 1;
	draw_list_buffer_copy(draw_list, num_points * 3, (double *)points);
	draw_list_append(draw_list, PRIMITIVE_TYPE_POINT, num_points * 3);
	;
	return 0;
}

int draw_list_lines(draw_list_t *draw_list, int num_lines, double *lines)
{
	if (num_lines < 1)
		return 1;
	draw_list_buffer_copy(draw_list, num_lines * 6, (double *)lines);
	draw_list_append(draw_list, PRIMITIVE_TYPE_LINE, num_lines * 6);
	return 0;
}

int draw_list_line(draw_list_t *draw_list, double x1, double y1, double z1,
		   double x2, double y2, double z2)
{
	double points[2][3] = { { x1, y1, z1 }, { x2, y2, z2 } };
	draw_list_buffer_copy(draw_list, 6, (double *)points);
	draw_list_append(draw_list, PRIMITIVE_TYPE_LINE, 6);
	return 0;
}

int draw_list_point(draw_list_t *draw_list, double x, double y, double z)
{
	double point[3] = { x, y, z };
	draw_list_buffer_copy(draw_list, 3, (double *)point);
	draw_list_append(draw_list, PRIMITIVE_TYPE_POINT, 3);
	return 0;
}

int draw_list_polygon(draw_list_t *draw_list, int num_points, double *points)
{
	if (num_points < 3)
		return 1;
	draw_list_buffer_copy(draw_list, num_points * 3, (double *)points);
	draw_list_append(draw_list, PRIMITIVE_TYPE_POLYGON, num_points * 3);
	return 0;
}

int draw_list_polyline(draw_list_t *draw_list, int num_points, double *points)
{
	if (num_points < 2)
		return 1;
	draw_list_buffer_copy(draw_list, num_points * 3, (double *)points);
	draw_list_append(draw_list, PRIMITIVE_TYPE_POLYLINE, num_points * 3);
	return 0;
}

int draw_list_style(draw_list_t *draw_list, double color[4], double width)
{
	draw_list_buffer_copy(draw_list, 4, color);
	draw_list_buffer_copy(draw_list, 1, &width);
	draw_list_append(draw_list, PRIMITIVE_TYPE_STYLE, 5);
	return 0;
}

int draw_list_style2(draw_list_t *draw_list, double r, double g, double b,
		     double a, double width)
{
	double color[4] = { r, g, b, a };
	draw_list_buffer_copy(draw_list, 4, color);
	draw_list_buffer_copy(draw_list, 1, &width);
	draw_list_append(draw_list, PRIMITIVE_TYPE_STYLE, 5);
	return 0;
}

int draw_list_clear(draw_list_t *draw_list)
{
	draw_list_append(draw_list, PRIMITIVE_TYPE_CLEAR, 0);
	return 0;
}

void _draw_list_render_line(draw_list_t *draw_list, primitive_t *primitive,
			    cairo_t *cr, camera_t *camera)
{
	double *points = draw_list->buffer + primitive->index;
	size_t num = primitive->length / 6;
	double p1[2], p2[2];
	bool b1, b2;
	for (int i = 0; i < num; i++) {
		b1 = camera_project(camera, &points[i * 6], p1);
		b2 = camera_project(camera, &points[i * 6 + 3], p2);
		if (!b1 || !b2)
			continue;
		cairo_move_to(cr, p1[0], p1[1]);
		cairo_line_to(cr, p2[0], p2[1]);
		cairo_stroke(cr);
	}
}

void _draw_list_render_point(draw_list_t *draw_list, primitive_t *primitive,
			     cairo_t *cr, camera_t *camera)
{
	double *points = draw_list->buffer + primitive->index;
	size_t num_points = primitive->length / 3;
	double p[2];
	bool b;
	for (int i = 0; i < num_points; i++) {
		b = camera_project(camera, &points[i * 3], p);
		if (!b)
			continue;
		cairo_move_to(cr, p[0], p[1]);
		cairo_stroke(cr);
	}
}

void _draw_list_render_polygon(draw_list_t *draw_list, primitive_t *primitive,
			       cairo_t *cr, camera_t *camera)
{
	double *points = draw_list->buffer + primitive->index;
	size_t num_points = primitive->length / 3;
	double p[2];
	bool b = false;
	for (int i = 0; i < num_points; i++) {
		b = camera_project(camera, &points[i * 3], p);
		if (!b)
			continue;
		cairo_line_to(cr, p[0], p[1]);
	}
	if (b) {
		cairo_close_path(cr);
		cairo_fill(cr);
	} else {
		cairo_stroke(cr);
	}
}

void _draw_list_render_polyline(draw_list_t *draw_list, primitive_t *primitive,
				cairo_t *cr, camera_t *camera)
{
	double *points = draw_list->buffer + primitive->index;
	size_t num_points = primitive->length / 3;
	double p[2];
	bool b1, b2;
	b1 = camera_project(camera, &points[0], p);
	for (int i = 0; i < num_points; i++) {
		b2 = camera_project(camera, &points[i * 3], p);
		if (b1 && b2) {
			cairo_line_to(cr, p[0], p[1]);
		} else {
			cairo_stroke(cr);
			cairo_move_to(cr, p[0], p[1]);
		}
		b1 = b2;
	}
	cairo_stroke(cr);
}

void _draw_list_render_style(draw_list_t *draw_list, primitive_t *primitive,
			     cairo_t *cr, camera_t *camera)
{
	double *color = draw_list->buffer + primitive->index;
	double width = draw_list->buffer[primitive->index + 4];
	cairo_set_source_rgba(cr, color[0], color[1], color[2], color[3]);
	cairo_set_line_width(cr, width);
}

void _draw_list_render_clear(draw_list_t *draw_list, primitive_t *primitive,
			     cairo_t *cr, camera_t *camera)
{
	// fill with memset
	cairo_rectangle(cr, 0, 0, camera->width, camera->height);
	cairo_fill(cr);
}

int draw_list_render(draw_list_t *draw_list, cairo_t *cr, camera_t *camera)
{
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	for (int i = 0; i < draw_list->length; i++) {
		primitive_t *primitive = &draw_list->primitives[i];
		switch (primitive->type) {
		case PRIMITIVE_TYPE_LINE:
			_draw_list_render_line(draw_list, primitive, cr,
					       camera);
			break;
		case PRIMITIVE_TYPE_POINT:
			_draw_list_render_point(draw_list, primitive, cr,
						camera);
			break;
		case PRIMITIVE_TYPE_POLYGON:
			_draw_list_render_polygon(draw_list, primitive, cr,
						  camera);
			break;
		case PRIMITIVE_TYPE_POLYLINE:
			_draw_list_render_polyline(draw_list, primitive, cr,
						   camera);
			break;
		case PRIMITIVE_TYPE_STYLE:
			_draw_list_render_style(draw_list, primitive, cr,
						camera);
			break;
		case PRIMITIVE_TYPE_CLEAR:
			_draw_list_render_clear(draw_list, primitive, cr,
						camera);
			break;
		default:
			break;
		}
	}
	return 0;
}

int draw_list_render_svg(draw_list_t *draw_list, const char *filename,
			 camera_t *camera)
{
	int width, height;
	camera_viewport_get(camera, &width, &height);
	cairo_surface_t *surface =
		cairo_svg_surface_create(filename, width, height);
	cairo_t *cr = cairo_create(surface);
	draw_list_render(draw_list, cr, camera);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	return 0;
}

camera_t *camera_create()
{
	camera_t *camera = malloc(sizeof(camera_t));
	camera->width = 1.;
	camera->height = 1.;
	camera->distance = 0.0;
	for (int i = 0; i < 3; i++) {
		camera->position[i] = 0.0;
		camera->rotation[i] = 0.0;
	}
	for (int i = 0; i < 16; i++) {
		camera->m[i] = 0.0;
		camera->mi[i] = 0.0;
	}
	for (int i = 0; i < 4; i++) {
		camera->m[i * 4 + i] = 1.0;
		camera->mi[i * 4 + i] = 1.0;
	}
	camera->preserve_ratio = true;
	camera->ratio = 1.;
	return camera;
}

int camera_destroy(camera_t *camera)
{
	free(camera);
	return 0;
}

int camera_position_set(camera_t *camera, double x, double y, double z)
{
	camera->position[0] = x;
	camera->position[1] = y;
	camera->position[2] = z;
	camera_update(camera);
	return 0;
}

int camera_position_get(camera_t *camera, double *x, double *y, double *z)
{
	*x = camera->position[0];
	*y = camera->position[1];
	*z = camera->position[2];
	return 0;
}

int camera_position_add(camera_t *camera, double x, double y, double z)
{
	camera->position[0] += x;
	camera->position[1] += y;
	camera->position[2] += z;
	camera_update(camera);
	return 0;
}

int camera_move(camera_t *camera, double x, double y, double z)
{
	// moves camera in the direction of the camera yaw (z axis)
	double cz = cos(camera->rotation[2]);
	double sz = sin(camera->rotation[2]);
	camera->position[0] += x * cz + y * sz;
	camera->position[1] += -x * sz + y * cz;
	camera->position[2] += z;
	camera_update(camera);
	return 0;
}

int camera_rotation_set(camera_t *camera, double x, double y, double z)
{
	camera->rotation[0] = x;
	camera->rotation[1] = y;
	camera->rotation[2] = z;
	camera_update(camera);
	return 0;
}

int camera_rotation_get(camera_t *camera, double *x, double *y, double *z)
{
	*x = camera->rotation[0];
	*y = camera->rotation[1];
	*z = camera->rotation[2];
	camera_update(camera);
	return 0;
}

int camera_rotation_add(camera_t *camera, double x, double y, double z)
{
	camera->rotation[0] += x;
	camera->rotation[1] += y;
	camera->rotation[2] += z;
	camera_update(camera);
	return 0;
}

int camera_distance_set(camera_t *camera, double distance)
{
	camera->distance = distance;
	camera_update(camera);
	return 0;
}

int camera_distance_get(camera_t *camera, double *distance)
{
	*distance = camera->distance;
	return 0;
}

int camera_distance_add(camera_t *camera, double distance)
{
	camera->distance += distance;
	camera_update(camera);
	return 0;
}

int camera_perspective(camera_t *camera, double hfov, double vfov)
{
	double fx = 0.5 / tan(hfov / 2.0);
	double fy = 0.5 / tan(vfov / 2.0);
	for (int i = 0; i < 16; i++) {
		camera->mi[i] = 0.0;
	}
	camera->mi[0 * 4 + 0] = fx;
	camera->mi[1 * 4 + 1] = fy;
	camera->mi[0 * 4 + 2] = 0.5;
	camera->mi[1 * 4 + 2] = 0.5;
	camera->mi[2 * 4 + 2] = 1.0;
	camera->mi[3 * 4 + 2] = 1.0;
	camera->ratio = fy / fx;
	camera_update(camera);
	return 0;
}

int camera_orthographic(camera_t *camera, double scale_x, double scale_y)
{
	for (int i = 0; i < 16; i++) {
		camera->mi[i] = 0.0;
	}
	camera->mi[0 * 4 + 0] = scale_x;
	camera->mi[1 * 4 + 1] = scale_y;
	camera->mi[2 * 4 + 2] = 1.0;
	camera->mi[3 * 4 + 3] = 1.0;
	camera->mi[0 * 4 + 3] = 0.5;
	camera->mi[1 * 4 + 3] = 0.5;
	camera->ratio = scale_y / scale_x;
	camera_update(camera);
	return 0;
}

int camera_viewport_set(camera_t *camera, int width, int height)
{
	camera->width = width;
	camera->height = height;
	camera_update(camera);
	return 0;
}

int camera_viewport_get(camera_t *camera, int *width, int *height)
{
	*width = camera->width;
	*height = camera->height;
	return 0;
}

int camera_projection_get(camera_t *camera, double m[16])
{
	memcpy(m, camera->m, sizeof(double) * 16);
	return 0;
}

int camera_projection_set(camera_t *camera, double m[16])
{
	memcpy(camera->m, m, sizeof(double) * 16);
	return 0;
}

bool camera_preserve_ratio_get(camera_t *camera)
{
	return camera->preserve_ratio;
}

int camera_preserve_ratio_set(camera_t *camera, bool preserve_ratio)
{
	camera->preserve_ratio = preserve_ratio;
	return 0;
}

bool camera_project(camera_t *camera, double *p, double *q)
{
	double p1[4] = { p[0], p[1], p[2], 1.0 };
	double p2[4] = { 0.0, 0.0, 0.0, 0.0 };
	matmul(camera->m, p1, p2, 4, 4, 1);
	q[0] = p2[0] / p2[3];
	q[1] = p2[1] / p2[3];
	return p2[3] > 0.0;
}

int camera_update(camera_t *camera) // will be...
{
	double a1[16];
	double a2[16];
	double a3[16];

	// a1 <- R
	// a2 <- T
	// a3 <- R * T (a1 * a2)
	// a1 <- Ts
	// a2 <- Ts * a3 (a1 * a3)
	// a1 <- permute
	// a3 <- permute * a2 (a1 * a2)
	// a1 <- eye
	// a2 <- eye * a3 (a1 * a3)
	// a1 <- Mi
	// a3 <- Mi * a3 (a1 * a2)
	// => a3 = Mi * eye * Ts * R * T
	rotxyz(a1, camera->rotation[0], camera->rotation[1],
	       camera->rotation[2]);
	translate(a2, -camera->position[0], -camera->position[1],
		  -camera->position[2]);
	matmul(a1, a2, a3, 4, 4, 4);
	translate(a1, camera->distance, 0.0, 0.0);
	matmul(a1, a3, a2, 4, 4, 4);
	permute(a1, 2, 0, 1, 3);
	matmul(a1, a2, a3, 4, 4, 4);
	scale(a1, 1.0, -1.0, 1.0, 1.0);
	matmul(a1, a3, a2, 4, 4, 4);
	memcpy(a1, camera->mi, sizeof(double) * 16);

	double new_ratio = camera->height / camera->width;
	double width = camera->width;
	double height = camera->height;
	if (camera->preserve_ratio) {
		if (new_ratio < camera->ratio) {
			width = camera->height / camera->ratio;
		} else {
			height = camera->width * camera->ratio;
		}
	}
	a1[0 * 4 + 0] *= width;
	a1[1 * 4 + 1] *= height;
	a1[0 * 4 + 2] *= camera->width;
	a1[1 * 4 + 2] *= camera->height;
	a1[0 * 4 + 3] *= camera->width;
	a1[1 * 4 + 3] *= camera->height;
	matmul(a1, a2, a3, 4, 4, 4);
	memcpy(camera->m, a3, sizeof(double) * 16);
	return 0;
}

int matmul(double *a, double *b, double *c, int n, int m, int r)
{
	// a: n x m
	// b: m x r
	// c: n x r
	for (int i = 0; i < n; i++) { // rows
		for (int j = 0; j < r; j++) { // columns
			c[i * r + j] = 0.0;
			for (int k = 0; k < m; k++) { // elements
				c[i * r + j] += a[i * m + k] * b[k * r + j];
			}
		}
	}
	return 0;
}

int rotxyz(double *m, double x, double y, double z)
{
	memset(m, 0, sizeof(double) * 16);

	double cz = cos(z); // cos(yaw)
	double sz = sin(z); // sin(yaw)
	double cy = cos(y); // cos(pitch)
	double sy = sin(y); // sin(pitch)
	double cx = cos(x); // cos(roll)
	double sx = sin(x); // sin(roll)

	// first roll (x), then pitch (y), then yaw (z)
	m[0 * 4 + 0] = cy * cz;
	m[0 * 4 + 1] = -cy * sz;
	m[0 * 4 + 2] = sy;
	m[0 * 4 + 3] = 0;

	m[1 * 4 + 0] = sx * sy * cz + cx * sz;
	m[1 * 4 + 1] = -sx * sy * sz + cx * cz;
	m[1 * 4 + 2] = -sx * cy;
	m[1 * 4 + 3] = 0;

	m[2 * 4 + 0] = -cx * sy * cz + sx * sz;
	m[2 * 4 + 1] = cx * sy * sz + sx * cz;
	m[2 * 4 + 2] = cx * cy;
	m[2 * 4 + 3] = 0;

	m[3 * 4 + 0] = 0;
	m[3 * 4 + 1] = 0;
	m[3 * 4 + 2] = 0;
	m[3 * 4 + 3] = 1;

	return 0;
}

int translate(double *m, double x, double y, double z)
{
	memset(m, 0, sizeof(double) * 16);
	m[0 * 4 + 0] = 1.0;
	m[1 * 4 + 1] = 1.0;
	m[2 * 4 + 2] = 1.0;
	m[3 * 4 + 3] = 1.0;
	m[0 * 4 + 3] = x;
	m[1 * 4 + 3] = y;
	m[2 * 4 + 3] = z;
	return 0;
}

int eye(double *m, int n)
{
	memset(m, 0, sizeof(double) * n * n);
	for (int i = 0; i < n; i++) {
		m[i * n + i] = 1.0;
	}
	return 0;
}

int permute(double *m, int i0, int i1, int i2, int i3)
{
	memset(m, 0, sizeof(double) * 16);
	m[i0 * 4 + 0] = 1.0;
	m[i1 * 4 + 1] = 1.0;
	m[i2 * 4 + 2] = 1.0;
	m[i3 * 4 + 3] = 1.0;
	return 0;
}

int scale(double *m, double x, double y, double z, double w)
{
	memset(m, 0, sizeof(double) * 16);
	m[0 * 4 + 0] = x;
	m[1 * 4 + 1] = y;
	m[2 * 4 + 2] = z;
	m[3 * 4 + 3] = w;
	return 0;
}