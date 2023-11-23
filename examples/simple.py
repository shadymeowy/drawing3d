from drawing3d import Simple3D

simple = Simple3D((720, 720), 45.0, "test")
simple.camera.position = (0.5, 0.5, 0.0)
simple.camera.distance = 5.0

while not simple.quiting:
    with simple as draw:
        draw.style2(1.0, 0.0, 0.0, 1.0, 8.0)
        lines = [
            [[0.0, 0.0, 0.0], [0.0, 1.0, 0.0]],
            [[0.0, 1.0, 0.0], [1.0, 1.0, 0.0]],
            [[1.0, 1.0, 0.0], [1.0, 0.0, 0.0]],
            [[1.0, 0.0, 0.0], [0.0, 0.0, 0.0]],
        ]
        draw.lines(lines)