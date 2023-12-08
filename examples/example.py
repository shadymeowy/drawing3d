from drawing3d import DrawList, Window, EventList
import numpy as np
import time

# Create two windows
windows = Window(720, 720, b"test"), Window(720, 720, b"test2")

# Set perspective and orthographic cameras
camera1 = windows[0].get_camera()
camera1.set_perspective(45.0, 45.0)

camera2 = windows[1].get_camera()
camera2.set_orthographic(1/12, 1/12)

# Set position and rotation of the second camera
camera2.position = (0., 0.0, 1.0)
camera2.rotation = (0.0, -np.pi/2, 0.0)
# Disable camera control for the second window
windows[1].controllable = False

# Create two draw lists
# One is for static "background" drawing
# The other is for dynamic drawing
draw_lists = [DrawList(), DrawList()]
draw_list_bg, draw_list = draw_lists

# Set background color
draw_list_bg.style2(1.0, 1.0, 1.0, 1.0, 1.0)
draw_list_bg.clear()

# Draw some shapes
draw_list_bg.style2(1.0, 0.0, 0.0, 1.0, 4.0)
draw_list_bg.line(0.0, 0.0, 0.0, 1.0, 1.0, 1.0)
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

# Draw a circle using numpy
t = np.linspace(0, 2 * np.pi, 100)
x = np.cos(t)
y = np.sin(t)
z = np.zeros_like(t)
points = np.stack((x, y, z), axis=1)
draw_list_bg.style2(0.0, 0.0, 1.0, 1.0, 5.0)
draw_list_bg.polyline(points)

# Draw some random points
random_points = np.random.rand(100, 3) * 2
random_points[:, 2] = 0.0
draw_list_bg.style2(0.0, 0.0, 1.0, 1.0, 8.0)
draw_list_bg.points(random_points)

# Create an event list and keep track of time
# for animation purposes
event_list = EventList()
stime = time.time()
prev_t = 0.0

# Loop until one of the windows is closed
quiting = False
while not event_list.poll():
    t = time.time() - stime
    dt = t - prev_t
    prev_t = t
    # print('FPS: {:.2f}'.format(1.0 / dt))
    # Rotate the first camera slowly around z-axis
    camera1.add_rotation(0.0, 0.0, dt / 10)

    # Clear the dynamic draw list
    draw_list.empty()
    # Draw a rotating line inside the static circle
    draw_list.style2(0.0, 0.0, 0.0, 1.0, 5.0)
    draw_list.line(0.0, 0.0, 0.0, np.cos(t), np.sin(t), 0.0)

    # Draw a fancy polar curve
    t2 = np.linspace(0, 2 * np.pi, 512)
    x = np.cos(t2 + t) * (2 + np.cos(t2 * 6)) * (1.5 + 0.5*np.cos(t))
    y = np.sin(t2 + t) * (2 + np.cos(t2 * 6)) * (1.5 + 0.5*np.cos(t))
    z = np.zeros_like(t2)
    points = np.stack((x, y, z), axis=1)
    draw_list.style2(1.0, 0.0, 1.0, 1.0, 5.0)
    draw_list.polyline(points)

    # Render the draw lists on the windows
    # and handle events
    for window in windows:
        for drawlist in draw_lists:
            window.render(drawlist)
        window.render_end()
        if window.handle_events(event_list):
            quiting = True
    if quiting:
        break
    # Clear the event list
    event_list.reset()

# Save the lastest draw lists as SVG files
DrawList.saves_svg(draw_lists, b"test.svg", camera1)
DrawList.saves_svg(draw_lists, b"test2.svg", camera2)

# Clean up
for window in windows:
    window.destroy()
for draw_list in draw_lists:
    draw_list.destroy()
event_list.destroy()
