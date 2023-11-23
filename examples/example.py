from drawing3d import DrawList, Window, EventList
import numpy as np

draw_list = DrawList()
window = Window(720, 720, b"test")
camera = window.get_camera()
camera.set_perspective(45.0, 45.0)

draw_list.style2(1.0, 1.0, 1.0, 1.0, 1.0)
draw_list.clear()

draw_list.style2(1.0, 0.0, 0.0, 1.0, 4.0)
draw_list.line(0.0, 0.0, 0.0, 1.0, 1.0, 1.0)

# cube
draw_list.style2(0.0, 1.0, 0.0, 1.0, 4.0)
lines = np.array([
    [[0.0, 0.0, 0.0], [0.0, 1.0, 0.0]],
    [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0]],
    [[0.0, 0.0, 0.0], [0.0, 0.0, 1.0]],
    [[1.0, 1.0, 1.0], [0.0, 1.0, 1.0]],
    [[1.0, 1.0, 1.0], [1.0, 0.0, 1.0]],
    [[1.0, 1.0, 1.0], [1.0, 1.0, 0.0]],
    [[0.0, 1.0, 0.0], [1.0, 1.0, 0.0]],
    [[0.0, 1.0, 0.0], [0.0, 1.0, 1.0]],
    [[1.0, 0.0, 0.0], [1.0, 0.0, 1.0]],
    [[1.0, 0.0, 0.0], [1.0, 1.0, 0.0]],
    [[0.0, 0.0, 1.0], [1.0, 0.0, 1.0]],
    [[0.0, 0.0, 1.0], [0.0, 1.0, 1.0]],
])
draw_list.lines(lines)

t = np.linspace(0, 2 * np.pi, 100)
x = np.cos(t)
y = np.sin(t)
z = np.zeros_like(t)
points = np.stack((x, y, z), axis=1)
draw_list.style2(0.0, 0.0, 1.0, 1.0, 5.0)
draw_list.polyline(points)

event_list = EventList()
while not event_list.poll():
    window.clear()
    window.render(draw_list)
    window.handle_events(event_list)
    event_list.reset()

window.destroy()
draw_list.destroy()
event_list.destroy() 
