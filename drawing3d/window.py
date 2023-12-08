from ._drawing3d import ffi, lib
from .helpers import buffer_from
from .camera import Camera


class Window:
    def __init__(self, width, height, title):
        self.obj = lib.window_create(width, height, title)

    def destroy(self):
        return lib.window_destroy(self.obj)

    def surface_init(self):
        return lib.window_surface_init(self.obj)

    def surface_destroy(self):
        return lib.window_surface_destroy(self.obj)

    def clear(self):
        return lib.window_clear(self.obj)

    def render(self, draw_list):
        return lib.window_render(self.obj, draw_list.obj)

    def render_end(self):
        return lib.window_render_end(self.obj)

    def render_at(self, draw_list, pos, att):
        x, y, z = pos
        rx, ry, rz = att
        return lib.window_render_at(self.obj, draw_list.obj, x, y, z, rx, ry, rz)

    def handle_events(self, event_list):
        return lib.window_handle_events(self.obj, event_list.obj)

    def do_key_action(self, action):
        return lib.window_do_key_action(self.obj, action)

    def get_camera(self):
        return Camera(obj=lib.window_camera_get(self.obj))

    def save_png(self, filename):
        return lib.window_save_png(self.obj, filename)

    def save(self, buffer, size):
        buffer = buffer_from("uint8_t[]", buffer)
        return lib.window_save(self.obj, buffer, size)

    @property
    def controllable(self):
        return bool(lib.window_controllable_get(self.obj))

    @controllable.setter
    def controllable(self, controllable):
        lib.window_controllable_set(self.obj, bool(controllable))

    @property
    def mouse_sensitivity(self):
        return lib.window_mouse_sensitivity_get(self.obj)

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, sensitivity):
        lib.window_mouse_sensitivity_set(self.obj, sensitivity)

    @property
    def wheel_sensitivity(self):
        return lib.window_wheel_sensitivity_get(self.obj)

    @wheel_sensitivity.setter
    def wheel_sensitivity(self, sensitivity):
        lib.window_wheel_sensitivity_set(self.obj, sensitivity)

    @property
    def key_sensitivity(self):
        return lib.window_key_sensitivity_get(self.obj)

    @key_sensitivity.setter
    def key_sensitivity(self, sensitivity):
        lib.window_key_sensitivity_set(self.obj, sensitivity)
