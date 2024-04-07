from cffi import FFI
import subprocess

ffibuilder = FFI()
ffibuilder.cdef(open("include/cffi_header.h").read())


def get_output(cmd):
    return subprocess.run(cmd, stdout=subprocess.PIPE).stdout.decode("utf-8").strip()


cflags = ["-O2"]
cflags += get_output(["sdl2-config", "--cflags"]).split()
cflags += get_output(["pkg-config", "--cflags", "cairo"]).split()


ldflags = ["-O2", "-ldrawing3d"]
ldflags += get_output(["sdl2-config", "--libs"]).split()
ldflags += get_output(["pkg-config", "--libs", "cairo"]).split()

# check platform to emit fpic or equivalent
if "linux" in get_output(["uname", "-s"]).lower():
    cflags.append("-fPIC")

ffibuilder.set_source(
    "drawing3d._drawing3d",
    "#include \"drawing3d.h\"",
    libraries=["SDL2", "cairo"],
    library_dirs=["./build"],
    runtime_library_dirs=["./build"],
    extra_compile_args=cflags,
    extra_link_args=ldflags,
    include_dirs=["./include"],
)

if __name__ == "__main__":
    ffibuilder.compile(verbose=True, tmpdir="build")
