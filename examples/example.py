from drawing3d import DrawList, Window, EventList
import numpy as np
import time

windows = Window(720, 720, b"test"), Window(720, 720, b"test2")

camera1 = windows[0].get_camera()
camera1.set_perspective(45.0, 45.0)

windows[1].controllable = False
camera2 = windows[1].get_camera()
camera2.set_orthographic(0.1, 0.1)
camera2.position = (0., 0.0, 1.0)
camera2.rotation = (0.0, -np.pi/2, 0.0)

draw_lists = [DrawList(), DrawList()]
draw_list_bg, draw_list = draw_lists

draw_list_bg.style2(1.0, 1.0, 1.0, 1.0, 1.0)
draw_list_bg.clear()

draw_list_bg.style2(1.0, 0.0, 0.0, 1.0, 4.0)
draw_list_bg.line(0.0, 0.0, 0.0, 1.0, 1.0, 1.0)

# cube
draw_list_bg.style2(0.0, 1.0, 0.0, 1.0, 4.0)
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
draw_list_bg.lines(lines)

t = np.linspace(0, 2 * np.pi, 100)
x = np.cos(t)
y = np.sin(t)
z = np.zeros_like(t)
points = np.stack((x, y, z), axis=1)
draw_list_bg.style2(0.0, 0.0, 1.0, 1.0, 5.0)
draw_list_bg.polyline(points)

event_list = EventList()
stime = time.time()
prev_t = 0.0

while not event_list.poll():
    t = time.time() - stime
    dt = t - prev_t
    prev_t = t
    print('FPS: {:.2f}'.format(1.0 / dt))
    camera1.add_rotation(0.0, 0.0, dt / 10)

    draw_list.empty()
    draw_list.style2(0.0, 0.0, 0.0, 1.0, 5.0)
    draw_list.line(0.0, 0.0, 0.0, np.cos(t), np.sin(t), 0.0)

    t2 = np.linspace(0, 2 * np.pi, 512)
    # polar flower
    x = np.cos(t2 + t) * (2 + np.cos(t2 * 6)) * (1.25 + np.cos(t))
    y = np.sin(t2 + t) * (2 + np.cos(t2 * 6)) * (1.25 + np.cos(t))
    z = np.zeros_like(t2)
    points = np.stack((x, y, z), axis=1)
    draw_list.style2(1.0, 0.0, 1.0, 1.0, 5.0)
    draw_list.polyline(points)

    for window in windows:
        window.renders(draw_lists)
        window.handle_events(event_list)
    event_list.reset()


for window in windows:
    window.destroy()
for draw_list in draw_lists:
    draw_list.destroy()
event_list.destroy()
