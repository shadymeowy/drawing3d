from ._drawing3d import ffi, lib
from .helpers import *


class DrawList:
    def __init__(self, obj=None):
        if obj is None:
            obj = lib.draw_list_create()
        self.obj = obj

    def destroy(self):
        return lib.draw_list_destroy(self.obj)

    def save(self):
        return lib.draw_list_save(self.obj)

    def load(self):
        return lib.draw_list_load(self.obj)

    def empty(self):
        return lib.draw_list_empty(self.obj)

    def buffer_allocate(self, num):
        return lib.draw_list_buffer_allocate(self.obj, num)

    def buffer_copy(self, num, src):
        src = buffer_from("double[]", src)
        return lib.draw_list_buffer_copy(self.obj, num, src)

    def append(self, type, num):
        return lib.draw_list_append(self.obj, type, num)

    def points(self, points):
        num_points, points = points_from_np(points)
        return lib.draw_list_points(self.obj, num_points, points)

    def lines(self, lines):
        num_lines, lines = lines_from_np(lines)
        return lib.draw_list_lines(self.obj, num_lines, lines)

    def line(self, x1, y1, z1, x2, y2, z2):
        return lib.draw_list_line(self.obj, x1, y1, z1, x2, y2, z2)

    def point(self, x, y, z):
        return lib.draw_list_point(self.obj, x, y, z)

    def polygon(self, points):
        num_points, points = points_from_np(points)
        return lib.draw_list_polygon(self.obj, num_points, points)

    def polyline(self, points):
        num_points, points = points_from_np(points)
        return lib.draw_list_polyline(self.obj, num_points, points)

    def style(self, color, width):
        num, color = doubles_from_np(color)
        if num != 4:
            raise ValueError("color must be a 4 length array or equivalent")
        return lib.draw_list_style(self.obj, color, width)

    def style2(self, r, g, b, a, width):
        return lib.draw_list_style2(self.obj, r, g, b, a, width)

    def clear(self):
        return lib.draw_list_clear(self.obj)

    def render(self, cr, camera):
        return lib.draw_list_render(self.obj, cr, camera.obj)

    def save_svg(self, filename, camera):
        return lib.draw_list_save_svg(self.obj, filename, camera.obj)

    @classmethod
    def saves_svg(cls, draw_lists, filename, camera):
        num = len(draw_lists)
        draw_lists = ffi.new("draw_list_t *[]", [dl.obj for dl in draw_lists])
        return lib.draw_list_saves_svg(num, draw_lists, filename, camera.obj)

    def save_png(self, filename, camera):
        return lib.draw_list_save_png(self.obj, filename, camera.obj)

    @classmethod
    def saves_png(cls, draw_lists, filename, camera):
        num = len(draw_lists)
        draw_lists = ffi.new("draw_list_t *[]", [dl.obj for dl in draw_lists])
        return lib.draw_list_saves_png(num, draw_lists, filename, camera.obj)

    def save_buffer(self, camera):
        width, height = camera.viewport
        buffer = np.zeros((height, width, 4), dtype=np.uint8)
        buffer_ffi = ffi.from_buffer("uint8_t*", buffer)
        lib.draw_list_save_buffer(self.obj, buffer_ffi, camera.obj)
        return buffer

    @classmethod
    def saves_buffer(cls, draw_lists, camera):
        width, height = camera.viewport
        buffer = np.zeros((height, width, 4), dtype=np.uint8)
        buffer_ffi = ffi.from_buffer("uint8_t*", buffer)
        num = len(draw_lists)
        draw_lists = ffi.new("draw_list_t *[]", [dl.obj for dl in draw_lists])
        lib.draw_list_saves_buffer(num, draw_lists, buffer_ffi, camera.obj)
        return buffer
