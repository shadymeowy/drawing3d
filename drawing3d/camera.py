from ._drawing3d import ffi, lib
from .helpers import buffer_from


class Camera:
    def __init__(self, obj=None):
        if obj is None:
            obj = lib.camera_create()
        self.obj = obj

    def destroy(self):
        return lib.camera_destroy(self.obj)

    @property
    def position(self):
        x = ffi.new("double*")
        y = ffi.new("double*")
        z = ffi.new("double*")
        lib.camera_position_get(self.obj, x, y, z)
        return x[0], y[0], z[0]

    @position.setter
    def position(self, rot):
        lib.camera_position_set(self.obj, *rot)

    def add_position(self, x, y, z):
        return lib.camera_position_add(self.obj, x, y, z)

    def move(self, x, y, z):
        return lib.camera_move(self.obj, x, y, z)

    @property
    def rotation(self):
        x = ffi.new("double*")
        y = ffi.new("double*")
        z = ffi.new("double*")
        lib.camera_rotation_get(self.obj, x, y, z)
        return x[0], y[0], z[0]

    @rotation.setter
    def rotation(self, rot):
        lib.camera_rotation_set(self.obj, *rot)

    def add_rotation(self, x, y, z):
        return lib.camera_rotation_add(self.obj, x, y, z)

    @property
    def world_position(self):
        x = ffi.new("double*")
        y = ffi.new("double*")
        z = ffi.new("double*")
        lib.camera_world_position_get(self.obj, x, y, z)
        return x[0], y[0], z[0]

    @world_position.setter
    def world_position(self, rot):
        lib.camera_world_position_set(self.obj, *rot)

    def add_world_position(self, x, y, z):
        return lib.camera_world_position_add(self.obj, x, y, z)

    @property
    def world_rotation(self):
        x = ffi.new("double*")
        y = ffi.new("double*")
        z = ffi.new("double*")
        lib.camera_world_rotation_get(self.obj, x, y, z)
        return x[0], y[0], z[0]

    @world_rotation.setter
    def world_rotation(self, rot):
        lib.camera_world_rotation_set(self.obj, *rot)

    def add_world_rotation(self, x, y, z):
        return lib.camera_world_rotation_add(self.obj, x, y, z)

    @property
    def distance(self):
        distance = ffi.new("double*")
        lib.camera_distance_get(self.obj, distance)
        return distance[0]

    @distance.setter
    def distance(self, distance):
        lib.camera_distance_set(self.obj, distance)

    def add_distance(self, distance):
        return lib.camera_distance_add(self.obj, distance)

    def set_perspective(self, hfov, vfov):
        return lib.camera_perspective(self.obj, hfov, vfov)

    def set_orthographic(self, scale_x, scale_y):
        return lib.camera_orthographic(self.obj, scale_x, scale_y)

    @property
    def viewport(self):
        width = ffi.new("int*")
        height = ffi.new("int*")
        lib.camera_viewport_get(self.obj, width, height)
        return width[0], height[0]

    @viewport.setter
    def viewport(self, viewport):
        width, height = viewport
        lib.camera_viewport_set(self.obj, width, height)

    def preserve_ratio_get(self):
        return bool(lib.camera_preserve_ratio_get(self.obj))

    def preserve_ratio_set(self, preserve_ratio):
        preserve_ratio = bool(preserve_ratio)
        return lib.camera_preserve_ratio_set(self.obj, preserve_ratio)

    @property
    def projection(self):
        m = ffi.new("double[16]")
        lib.camera_projection_get(self.obj, m)
        return [m[i] for i in range(16)]

    @projection.setter
    def projection(self, m):
        m = buffer_from("double[]", m)
        lib.camera_projection_set(self.obj, m)

    def project(self, p1, p2):
        p1 = buffer_from("double[]", p1)
        p2 = buffer_from("double[]", p2)
        return lib.camera_project(self.obj, p1, p2)

    def update(self):
        return lib.camera_update(self.obj)
