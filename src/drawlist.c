#include "drawlist.h"

#include <string.h>
#include <stdlib.h>

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
	for (int i = 0; i < (int)num; i++) {
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
	for (int i = 0; i < (int)num_points; i++) {
		b = camera_project(camera, &points[i * 3], p);
		if (!b)
			continue;
		cairo_move_to(cr, p[0], p[1]);
		cairo_line_to(cr, p[0], p[1]);
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
	for (int i = 0; i < (int)num_points; i++) {
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
	for (int i = 0; i < (int)num_points; i++) {
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
	(void)camera;
	double *color = draw_list->buffer + primitive->index;
	double width = draw_list->buffer[primitive->index + 4];
	cairo_set_source_rgba(cr, color[0], color[1], color[2], color[3]);
	cairo_set_line_width(cr, width);
}

void _draw_list_render_clear(draw_list_t *draw_list, primitive_t *primitive,
			     cairo_t *cr, camera_t *camera)
{
	(void)primitive;
	(void)camera;
	(void)draw_list;
	int width, height;
	camera_viewport_get(camera, &width, &height);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);
}

int draw_list_render(draw_list_t *draw_list, cairo_t *cr, camera_t *camera)
{
	camera_update(camera);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	for (int i = 0; i < (int)draw_list->length; i++) {
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

int draw_list_save_svg(draw_list_t *draw_list, const char *filename,
		       camera_t *camera)
{
	draw_list_t *draw_lists[1] = { draw_list };
	return draw_list_saves_svg(1, draw_lists, filename, camera);
}

int draw_list_saves_svg(size_t num, draw_list_t **draw_list,
			const char *filename, camera_t *camera)
{
	int width, height;
	camera_viewport_get(camera, &width, &height);
	cairo_surface_t *surface =
		cairo_svg_surface_create(filename, width, height);
	cairo_t *cr = cairo_create(surface);
	for (int i = 0; i < (int)num; i++) {
		draw_list_render(draw_list[i], cr, camera);
	}
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	return 0;
}

int draw_list_save_png(draw_list_t *draw_list, const char *filename,
		       camera_t *camera)
{
	draw_list_t *draw_lists[1] = { draw_list };
	return draw_list_saves_png(1, draw_lists, filename, camera);
}

int draw_list_saves_png(size_t num, draw_list_t **draw_list,
			const char *filename, camera_t *camera)
{
	int width, height;
	camera_viewport_get(camera, &width, &height);
	cairo_surface_t *surface =
		cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cairo_t *cr = cairo_create(surface);
	for (int i = 0; i < (int)num; i++) {
		draw_list_render(draw_list[i], cr, camera);
	}
	cairo_surface_write_to_png(surface, filename);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	return 0;
}

int draw_list_save_buffer(draw_list_t *draw_list, uint8_t *buffer,
			  camera_t *camera)
{
	draw_list_t *draw_lists[1] = { draw_list };
	return draw_list_saves_buffer(1, draw_lists, buffer, camera);
}

int draw_list_saves_buffer(size_t num, draw_list_t **draw_list,
			   uint8_t *buffer, camera_t *camera)
{
	int width, height;
	camera_viewport_get(camera, &width, &height);
	cairo_surface_t *surface =
		cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32,
						    width, height, width * 4);
	cairo_t *cr = cairo_create(surface);
	for (int i = 0; i < (int)num; i++) {
		draw_list_render(draw_list[i], cr, camera);
	}
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	return 0;
}