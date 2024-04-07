#include "camera.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

struct camera_s {
	// viewport
	double width;
	double height;
	// extrinsic parameters
	double position[3];
	double rotation[3];
	// "object" position and rotation
	double wposition[3];
	double wrotation[3];
	double distance; // for spherical camera
	// intrinsic parameters, homogeneous
	double mi[16];
	// projection matrix
	double m[16];
	bool preserve_ratio;
	double ratio;
};

static int matmul(double *a, double *b, double *c, int n, int m, int r);
static int rotxyz(double *m, double x, double y, double z);
static int translate(double *m, double x, double y, double z);
static int eye(double *m, int n);
static int permute(double *m, int i0, int i1, int i2, int i3);

camera_t *camera_create()
{
	camera_t *camera = malloc(sizeof(camera_t));
	camera->width = 1.;
	camera->height = 1.;
	camera->distance = 0.0;
	for (int i = 0; i < 3; i++) {
		camera->position[i] = 0.0;
		camera->rotation[i] = 0.0;
		camera->wposition[i] = 0.0;
		camera->wrotation[i] = 0.0;
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
	return 0;
}

int camera_rotation_set(camera_t *camera, double x, double y, double z)
{
	camera->rotation[0] = x;
	camera->rotation[1] = y;
	camera->rotation[2] = z;
	return 0;
}

int camera_rotation_get(camera_t *camera, double *x, double *y, double *z)
{
	*x = camera->rotation[0];
	*y = camera->rotation[1];
	*z = camera->rotation[2];
	return 0;
}

int camera_rotation_add(camera_t *camera, double x, double y, double z)
{
	camera->rotation[0] += x;
	camera->rotation[1] += y;
	camera->rotation[2] += z;
	return 0;
}

int camera_object_position_set(camera_t *camera, double x, double y, double z)
{
	camera->wposition[0] = x;
	camera->wposition[1] = y;
	camera->wposition[2] = z;
	return 0;
}

int camera_object_position_get(camera_t *camera, double *x, double *y, double *z)
{
	*x = camera->wposition[0];
	*y = camera->wposition[1];
	*z = camera->wposition[2];
	return 0;
}

int camera_object_position_add(camera_t *camera, double x, double y, double z)
{
	camera->wposition[0] += x;
	camera->wposition[1] += y;
	camera->wposition[2] += z;
	return 0;
}

int camera_object_rotation_set(camera_t *camera, double x, double y, double z)
{
	camera->wrotation[0] = x;
	camera->wrotation[1] = y;
	camera->wrotation[2] = z;
	return 0;
}

int camera_object_rotation_get(camera_t *camera, double *x, double *y, double *z)
{
	*x = camera->wrotation[0];
	*y = camera->wrotation[1];
	*z = camera->wrotation[2];
	return 0;
}

int camera_object_rotation_add(camera_t *camera, double x, double y, double z)
{
	camera->wrotation[0] += x;
	camera->wrotation[1] += y;
	camera->wrotation[2] += z;
	return 0;
}

int camera_distance_set(camera_t *camera, double distance)
{
	camera->distance = distance;
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
	camera->ratio = fx / fy;
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
	return 0;
}

int camera_viewport_set(camera_t *camera, int width, int height)
{
	camera->width = width;
	camera->height = height;
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

int camera_update(camera_t *camera)
{
	double acc[16];
	double a1[16];
	double a2[16];

	// Mi * permute * Ts * R^-1 * T^-1 * Tw * Rw
	eye(acc, 4);

	rotxyz(a1, camera->wrotation[0], camera->wrotation[1],
	       camera->wrotation[2]);
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);

	translate(a1, camera->wposition[0], camera->wposition[1],
		  camera->wposition[2]);
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);

	translate(a1, -camera->position[0], -camera->position[1],
		  -camera->position[2]);
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);

	rotxyz(a1, -camera->rotation[0], -camera->rotation[1],
	       -camera->rotation[2]);
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);

	translate(a1, camera->distance, 0.0, 0.0);
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);

	permute(a1, 2, 0, 1, 3);
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);

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
	memcpy(a2, acc, sizeof(double) * 16);
	matmul(a1, a2, acc, 4, 4, 4);
	memcpy(camera->m, acc, sizeof(double) * 16);
	return 0;
}

static int matmul(double *a, double *b, double *restrict c, int n, int m, int r)
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

static int rotxyz(double *m, double x, double y, double z)
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

static int translate(double *m, double x, double y, double z)
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

static int eye(double *m, int n)
{
	memset(m, 0, sizeof(double) * n * n);
	for (int i = 0; i < n; i++) {
		m[i * n + i] = 1.0;
	}
	return 0;
}

static int permute(double *m, int i0, int i1, int i2, int i3)
{
	memset(m, 0, sizeof(double) * 16);
	m[i0 * 4 + 0] = 1.0;
	m[i1 * 4 + 1] = 1.0;
	m[i2 * 4 + 2] = 1.0;
	m[i3 * 4 + 3] = 1.0;
	return 0;
}