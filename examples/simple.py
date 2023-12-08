from drawing3d import Simple3D

# Create a simple drawing context
# This will create a window with a camera and a draw list for us
simple = Simple3D((720, 720), 45.0, "test")
# Set position and rotation of the camera
simple.camera.position = (0.5, 0.5, 0.5)
simple.camera.rotation = (0.0, 1.0, 0.0)
# Spherical camera distance to the camera origin
simple.camera.distance = 5.0

# Loop until the window is closed
while not simple.quiting:
    # Acquire the draw list
    with simple as draw:
        # Draw a cube with a red outline
        draw.style2(1.0, 0.0, 0.0, 1.0, 8.0)
        lines = [
            [[0.0, 0.0, 0.0], [0.0, 1.0, 0.0]],
            [[0.0, 1.0, 0.0], [1.0, 1.0, 0.0]],
            [[1.0, 1.0, 0.0], [1.0, 0.0, 0.0]],
            [[1.0, 0.0, 0.0], [0.0, 0.0, 0.0]],
        ]
        draw.lines(lines)