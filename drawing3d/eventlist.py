from ._drawing3d import ffi, lib


class EventList:
    def __init__(self, obj=None):
        if obj is None:
            obj = lib.event_list_create()
        self.obj = obj

    def destroy(self):
        return lib.event_list_destroy(self.obj)

    def reset(self):
        return lib.event_list_reset(self.obj)

    def length(self):
        return lib.event_list_length(self.obj)

    def poll(self):
        return lib.event_list_poll(self.obj)
