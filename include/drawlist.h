#ifndef DRAWLIST_H
#define DRAWLIST_H

#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "camera.h"

struct draw_list_s;
typedef struct draw_list_s draw_list_t;
struct primitive_s;
typedef struct primitive_s primitive_t;

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

#endif
