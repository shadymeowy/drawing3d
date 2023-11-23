from setuptools import setup, find_packages

import subprocess
subprocess.run(["make", "all"])

setup(
    name="drawing3d",
    version="0.0.1",
    description="A drawing 3D library for easy visualization",
    url="https://github.com/shadymeowy/python-drawing3d",
    author="Tolga Demirdal",
    packages=find_packages(),
    install_requires=["cffi>=1.0.0"],
    setup_requires=["cffi>=1.0.0"],
    cffi_modules=["drawing3d/build_ffi.py:ffibuilder"]
)
