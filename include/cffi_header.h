typedef void cairo_t;
typedef void SDL_Event;

struct camera_s;
typedef struct camera_s camera_t;
struct primitive_s;
typedef struct primitive_s primitive_t;
struct draw_list_s;
typedef struct draw_list_s draw_list_t;

struct window_s;
typedef struct window_s window_t;
struct event_list_s;
typedef struct event_list_s event_list_t;

typedef enum {
	// a line segment between two points
	// 2 points (6 doubles)
	PRIMITIVE_TYPE_LINE,
	// a single point
	// 1 point (3 doubles)
	PRIMITIVE_TYPE_POINT,
	// a filled polygon with n points
	// n points (3 * n doubles)
	PRIMITIVE_TYPE_POLYGON,
	// a polyline with n points
	// n points (3 * n doubles)
	PRIMITIVE_TYPE_POLYLINE,
	// set the style
	// r, g, b, a, width (5 doubles)
	PRIMITIVE_TYPE_STYLE,
	// clear (0 doubles)
	PRIMITIVE_TYPE_CLEAR,
} primitive_type_t;

typedef uint16_t key_action_t;

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

draw_list_t *draw_list_create();
int draw_list_destroy(draw_list_t *draw_list);
int draw_list_save(draw_list_t *draw_list);
int draw_list_load(draw_list_t *draw_list);
int draw_list_empty(draw_list_t *draw_list);
int draw_list_buffer_allocate(draw_list_t *draw_list, size_t num);
int draw_list_buffer_copy(draw_list_t *draw_list, size_t num, double *src);
int draw_list_append(draw_list_t *draw_list, primitive_type_t type, size_t num);
int draw_list_points(draw_list_t *draw_list, int num_points, double *points);
int draw_list_lines(draw_list_t *draw_list, int num_lines, double *lines);
int draw_list_line(draw_list_t *draw_list, double x1, double y1, double z1,
		   double x2, double y2, double z2);
int draw_list_point(draw_list_t *draw_list, double x, double y, double z);
int draw_list_polygon(draw_list_t *draw_list, int num_points, double *points);
int draw_list_polyline(draw_list_t *draw_list, int num_points, double *points);
int draw_list_style(draw_list_t *draw_list, double color[4], double width);
int draw_list_style2(draw_list_t *draw_list, double r, double g, double b,
		     double a, double width);
int draw_list_clear(draw_list_t *draw_list);
int draw_list_render(draw_list_t *draw_list, cairo_t *cr, camera_t *camera);
int draw_list_save_svg(draw_list_t *draw_list, const char *filename,
		       camera_t *camera);
int draw_list_saves_svg(size_t num, draw_list_t **draw_list,
			const char *filename, camera_t *camera);
int draw_list_save_png(draw_list_t *draw_list, const char *filename,
		       camera_t *camera);
int draw_list_saves_png(size_t num, draw_list_t **draw_list,
			const char *filename, camera_t *camera);
int draw_list_save_buffer(draw_list_t *draw_list, uint8_t *buffer,
			  camera_t *camera);
int draw_list_saves_buffer(size_t num, draw_list_t **draw_list,
			   uint8_t *buffer, camera_t *camera);

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

event_list_t *event_list_create();
int event_list_destroy(event_list_t *event_list);
int event_list_reset(event_list_t *event_list);
int event_list_append(event_list_t *event_list, SDL_Event *event);
int event_list_length(event_list_t *event_list);
int event_list_get(event_list_t *event_list, int index, SDL_Event *event);
int event_list_poll(event_list_t *event_list);
