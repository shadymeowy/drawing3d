#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drawing3d.h"

#define PI 3.14159265358979323846

void draw_cube(draw_list_t *draw_list, double x0, double y0, double z0,
	       double x1, double y1, double z1)
{
	double lines[12][2][3] = {
		{ { x0, y0, z0 }, { x1, y0, z0 } },
		{ { x1, y0, z0 }, { x1, y1, z0 } },
		{ { x1, y1, z0 }, { x0, y1, z0 } },
		{ { x0, y1, z0 }, { x0, y0, z0 } },
		{ { x0, y0, z1 }, { x1, y0, z1 } },
		{ { x1, y0, z1 }, { x1, y1, z1 } },
		{ { x1, y1, z1 }, { x0, y1, z1 } },
		{ { x0, y1, z1 }, { x0, y0, z1 } },
		{ { x0, y0, z0 }, { x0, y0, z1 } },
		{ { x1, y0, z0 }, { x1, y0, z1 } },
		{ { x1, y1, z0 }, { x1, y1, z1 } },
		{ { x0, y1, z0 }, { x0, y1, z1 } },
	};
	draw_list_lines(draw_list, 12, (double *)lines);
}

void draw_sin(draw_list_t *draw_list, double x0, double y0, double z0,
	      double x1, double y1, double z1, double t)
{
	double points[512][3];
	for (int i = 0; i < 512; i++) {
		double x = x0 + (x1 - x0) * i / 512.0;
		double y = y0 + (y1 - y0) *
					(sin(x * 20. - t) -
					 sin(x * 20. * sqrt(2) + t) + 2) /
					4.0;
		double z = z0 + (z1 - z0) *
					(sin(x * 20. * sqrt(5) + t) -
					 sin(x * 20. * sqrt(3) - t) + 2) /
					4.0;
		points[i][0] = x;
		points[i][1] = y;
		points[i][2] = z;
	}
	draw_list_polyline(draw_list, 512, (double *)points);
}

int main()
{
	draw_list_t *draw_list_bg = draw_list_create();
	draw_list_style2(draw_list_bg, 1.0, 1.0, 1.0, 1.0, 16.0);
	draw_list_clear(draw_list_bg);

	draw_list_style2(draw_list_bg, 0.0, 1.0, 0.0, 1., 4.0);
	double points[4][3] = {
		{ -1.0, -1.0, -1.0 },
		{ 1.0, -1.0, -1.0 },
		{ 1.0, 1.0, -1.0 },
		{ -1.0, 1.0, -1.0 },
	};
	draw_list_polygon(draw_list_bg, 4, (double *)points);

	draw_list_style2(draw_list_bg, 1.0, 0.0, 0.0, 1.0, 4.0);
	draw_cube(draw_list_bg, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0);

	event_list_t *event_list = event_list_create();
	window_t *window1 = window_create(720, 720, "1");
	window_t *window2 = window_create(720, 720, "2");
	camera_t *camera1 = window_camera_get(window1);
	camera_t *camera2 = window_camera_get(window2);
	window_controllable_set(window2, false);

	camera_perspective(camera1, PI / 4.0, PI / 4.0);
	camera_position_set(camera1, 0.0, 0.0, 0.0);
	camera_distance_set(camera1, 0.0);
	camera_orthographic(camera2, 0.25, 0.25);
	camera_rotation_set(camera2, 0.0, -PI / 2.0, 0.0);

	draw_list_t *draw_list = draw_list_create();
	draw_list_t *draw_lists[2] = { draw_list_bg, draw_list };
	double t = 1;
	while (!event_list_poll(event_list)) {
		draw_list_empty(draw_list);
		draw_list_style2(draw_list, 0.0, 0.0, 1.0, 1.0, 4.0);
		draw_sin(draw_list, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, t);
		t += 1 / 120.0;
		for (int i = 0 ; i < 2; i++) {
			window_render(window1, draw_lists[i]);
			window_render(window2, draw_lists[i]);
		}
		window_render_end(window1);
		window_render_end(window2);

		if (window_handle_events(window1, event_list))
			break;
		if (window_handle_events(window2, event_list))
			break;
		event_list_reset(event_list);
	}

	// window_save_png(window1, "1.png");
	// window_save_png(window2, "2.png");
	// draw_list_save_svg(draw_list, "out1.svg", camera1);
	// draw_list_save_svg(draw_list, "out2.svg", camera2);
	// draw_list_destroy(draw_list);
	window_destroy(window1);
	window_destroy(window2);
	event_list_destroy(event_list);
	return 0;
}