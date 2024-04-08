from .camera import Camera
from .drawlist import DrawList
from .eventlist import EventList
from .window import Window
from .simple3d import Simple3D

try:
    from .version import version as __version__
except ImportError:
    # Fallback version if version.py is not generated
    __version__ = "0.0.0.dev0"
