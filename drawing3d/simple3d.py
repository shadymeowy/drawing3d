from .window import Window
from .eventlist import EventList
from .drawlist import DrawList


class Simple3D:
    def __init__(self, size=(640, 480), fov=60, title="Drawing3D"):
        self.window = Window(*size, title.encode())
        self.camera = self.window.get_camera()
        self.camera.set_perspective(fov, fov)
        self.draw_list = DrawList()
        self.event_list = EventList()
        self.quiting = False

        self.draw_list.style2(1.0, 1.0, 1.0, 1.0, 1.0)
        self.draw_list.clear()
        self.draw_list.save()

    def __enter__(self):
        self.draw_list.load()
        self.event_list.reset()
        self.event_list.poll()
        return self.draw_list

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.window.render(self.draw_list)
        self.window.render_end()
        if self.window.handle_events(self.event_list):
            self.quiting = True
