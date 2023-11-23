import numpy as np
from ._drawing3d import ffi


def is_buffer(obj):
    try:
        memoryview(obj)
    except TypeError:
        return False
    else:
        return True


def buffer_from(typ, obj):
    if is_buffer(obj):
        return ffi.from_buffer(typ, obj)
    else:
        return ffi.new(typ, obj)


def points_from_np(points):
    if not isinstance(points, np.ndarray):
        points = np.array(points, dtype=np.double)
    points = np.ascontiguousarray(points)
    if points.ndim != 2:
        raise ValueError("points must be 2D array")
    if points.shape[1] != 3:
        raise ValueError("the length of index 1 must be 3")
    num_points = points.shape[0]
    points = points.flatten()
    points = ffi.from_buffer("double[]", points)
    return num_points, points


def lines_from_np(lines):
    if not isinstance(lines, np.ndarray):
        lines = np.array(lines, dtype=np.double)
    lines = np.ascontiguousarray(lines)
    if lines.ndim != 3:
        raise ValueError("lines must be 3D array")
    if lines.shape[1] != 2:
        raise ValueError("the length of index 2 must be 2")
    if lines.shape[2] != 3:
        raise ValueError("the length of index 3 must be 3")
    num_lines = lines.shape[0]
    lines = lines.flatten()
    lines = ffi.from_buffer("double[]", lines)
    return num_lines, lines


def doubles_from_np(obj):
    if not isinstance(obj, np.ndarray):
        obj = np.array(obj, dtype=np.double)
    obj = np.ascontiguousarray(obj)
    if obj.ndim != 1:
        raise ValueError("obj must be 1D array")
    obj = ffi.from_buffer("double[]", obj)
    num = obj.shape[0]
    return num, obj