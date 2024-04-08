from setuptools import setup
import subprocess

subprocess.run(["make", "clean"])
subprocess.run(["rm", "drawing3d/_drawing3d.abi3.so"])
subprocess.run(["make", "all"])

setup(
    cffi_modules=["drawing3d/build_ffi.py:ffibuilder"]
)