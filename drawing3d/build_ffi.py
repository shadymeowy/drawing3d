from cffi import FFI

ffibuilder = FFI()
ffibuilder.cdef(open("include/cffi_header.h").read())

ffibuilder.set_source(
	"drawing3d._drawing3d",
	"#include \"drawing3d.h\"",
	libraries=["SDL2", "cairo"],
	library_dirs=["./build"],
	runtime_library_dirs=["./build"],
	extra_compile_args=["-O2"],
	extra_link_args=["-O2"],
	include_dirs=["./include"],
	extra_objects=["./build/libdrawing3d.a"]
)

if __name__ == "__main__":
	ffibuilder.compile(verbose=True, tmpdir="build")