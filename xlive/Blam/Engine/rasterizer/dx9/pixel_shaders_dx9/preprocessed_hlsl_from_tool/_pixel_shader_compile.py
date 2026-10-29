# Compiles the pixel shader sources in this folder the same way the Halo 2 Vista tool does and writes the C arrays and the replacement table to compiled\
# Needs Python 3 on Windows and d3dx9_31.dll (DirectX End-User Runtime), see !README.txt
import ctypes
import ctypes.wintypes as wintypes
import os
import re

# Set current working directory to the file's location
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

COMPILED_FOLDER = "compiled"

# tag name, implementation index, source file (tool output from tags\rasterizer\pixel_shaders_dx9), extra defines per variant
SOURCES = [
	("shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test", 1, "shaders_shader_passes_lightmap_lightmap_tex_detail_alpha_test_1.psh", ((), (), ())),
	("shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test_clamped", 1, "shaders_shader_passes_lightmap_lightmap_tex_detail_alpha_test_clamped_1.psh", ((), (), ())),
]

# same order as s_pixel_shader::compiled_shader
VARIANTS = [
	("ps_2_0", "ps_2_0", ()),
	("ps_2_0_zprime", "ps_2_0", ("ZPRIME",)),
	("ps_3_0_mrt", "ps_3_0", ("MRT",)),
]

# the compiler the tool uses, newer compilers (bin\fxc.exe) produce different code
d3dx = ctypes.WinDLL("d3dx9_31.dll")


class D3DXMACRO(ctypes.Structure):
	_fields_ = [("Name", ctypes.c_char_p), ("Definition", ctypes.c_char_p)]


D3DXCompileShader = d3dx.D3DXCompileShader
D3DXCompileShader.restype = ctypes.c_long
D3DXCompileShader.argtypes = [ctypes.c_char_p, wintypes.UINT, ctypes.POINTER(D3DXMACRO), ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p,
	wintypes.DWORD, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(ctypes.c_void_p), ctypes.c_void_p]


def read_buffer(buffer):
	# ID3DXBuffer vtable: 2 Release, 3 GetBufferPointer, 4 GetBufferSize
	vtable = ctypes.cast(ctypes.cast(buffer, ctypes.POINTER(ctypes.c_void_p))[0], ctypes.POINTER(ctypes.c_void_p))
	pointer = ctypes.WINFUNCTYPE(ctypes.c_void_p, ctypes.c_void_p)(vtable[3])(buffer)
	size = ctypes.WINFUNCTYPE(wintypes.DWORD, ctypes.c_void_p)(vtable[4])(buffer)
	data = ctypes.string_at(pointer, size)
	ctypes.WINFUNCTYPE(wintypes.ULONG, ctypes.c_void_p)(vtable[2])(buffer)
	return data


def compile_shader(source, profile, defines):
	macros = (D3DXMACRO * (len(defines) + 1))(*[D3DXMACRO(define.encode(), b"1") for define in defines], D3DXMACRO(None, None))
	shader = ctypes.c_void_p()
	errors = ctypes.c_void_p()
	result = D3DXCompileShader(source, len(source), macros, None, b"main", profile.encode(), 0, ctypes.byref(shader), ctypes.byref(errors), None)
	if result < 0:
		raise RuntimeError(read_buffer(errors).decode("latin1") if errors.value else hex(result & 0xFFFFFFFF))
	return read_buffer(shader)


# every '#case' of the source is one pixel shader of the implementation, the lines before the first one are for the tool only
def split_cases(text):
	parts = re.split(r"^#case [^\n]*\n", text, flags=re.M)
	if len(parts) > 1:
		return parts[1:]
	return [re.sub(r"^(;|#const)[^\n]*\n", "", parts[0], flags=re.M)]


def c_array(name, data):
	lines = ["static const unsigned char %s[] = {" % name]
	for i in range(0, len(data), 12):
		lines.append("  " + ", ".join("0x%02x" % b for b in data[i:i + 12]) + ("," if i + 12 < len(data) else ""))
	lines.append("};")
	return "\n".join(lines) + "\n"


def array_prefix(source_file):
	return "k_" + source_file[len("shaders_shader_passes_lightmap_"):-len(".psh")]


headers = []
tables = []
for tag_name, implementation_index, source_file, extra_defines in SOURCES:
	print(source_file)
	text = open(source_file, "r", encoding="latin1").read().replace("\r\n", "\n")
	prefix = array_prefix(source_file)
	header = os.path.join(COMPILED_FOLDER, prefix[2:] + ".h")

	arrays = ["#pragma once\n"]
	pixel_shaders = []
	for case_index, body in enumerate(split_cases(text)):
		names = []
		for variant_index, (suffix, profile, defines) in enumerate(VARIANTS):
			name = "%s_%d_%s" % (prefix, case_index, suffix)
			arrays.append(c_array(name, compile_shader(body.encode("latin1"), profile, defines + extra_defines[variant_index])))
			names.append(name)
		pixel_shaders.append(names)

	with open(header, "w", newline="\r\n") as file:
		file.write("\n".join(arrays))
	headers.append(header.replace("\\", "/"))
	tables.append((tag_name, implementation_index, prefix + "_pixel_shaders", pixel_shaders))

with open(os.path.join(COMPILED_FOLDER, "_compiled_shader_table.h"), "w", newline="\r\n") as file:
	file.write("#pragma once\n\n")
	file.write("/* enums */\n\n")
	file.write("// same order as s_pixel_shader::compiled_shader\n")
	file.write("enum e_compiled_pixel_shader_variant\n{\n")
	for suffix, profile, defines in VARIANTS:
		file.write("\t_compiled_pixel_shader_%s,\n" % suffix)
	file.write("\n\tk_compiled_pixel_shader_variant_count\n};\n\n")
	file.write("/* structures */\n\n")
	file.write("struct s_compiled_pixel_shader\n{\n")
	file.write("\tconst unsigned char* bytecode[k_compiled_pixel_shader_variant_count];\n")
	file.write("\tint16 size[k_compiled_pixel_shader_variant_count];\n};\n\n")
	file.write("struct s_pixel_shader_replacement_information\n{\n")
	file.write("\tconst char* tag_name;\n")
	file.write("\tint32 implementation_index;\n")
	file.write("\tint32 pixel_shader_count;\n")
	file.write("\tconst s_compiled_pixel_shader* pixel_shaders;\n};\n\n")
	file.write("/* constants */\n\n")
	file.write("extern const s_pixel_shader_replacement_information k_pixel_shader_replacement_information[%d];\n" % len(tables))

with open(os.path.join(COMPILED_FOLDER, "_compiled_shader_table.cpp"), "w", newline="\r\n") as file:
	file.write("#include \"stdafx.h\"\n")
	file.write("#include \"_compiled_shader_table.h\"\n\n")
	for header in headers:
		file.write("#include \"rasterizer/dx9/pixel_shaders_dx9/preprocessed_hlsl_from_tool/%s\"\n" % header)
	file.write("\n/* constants */\n")
	for tag_name, implementation_index, table_name, pixel_shaders in tables:
		file.write("\nstatic const s_compiled_pixel_shader %s[%d] =\n{\n" % (table_name, len(pixel_shaders)))
		for i, names in enumerate(pixel_shaders):
			file.write("\t{\n")
			file.write("\t\t{ %s },\n" % ", ".join(names))
			file.write("\t\t{ %s }\n" % ", ".join("sizeof(%s)" % name for name in names))
			file.write("\t}%s\n" % ("," if i + 1 < len(pixel_shaders) else ""))
		file.write("};\n")
	file.write("\nconst s_pixel_shader_replacement_information k_pixel_shader_replacement_information[%d] =\n{\n" % len(tables))
	for i, (tag_name, implementation_index, table_name, pixel_shaders) in enumerate(tables):
		file.write("\t{\n")
		file.write("\t\t\"%s\",\n" % tag_name.replace("\\", "\\\\"))
		file.write("\t\t%d,\n" % implementation_index)
		file.write("\t\tNUMBEROF(%s),\n" % table_name)
		file.write("\t\t%s\n" % table_name)
		file.write("\t}%s\n" % ("," if i + 1 < len(tables) else ""))
	file.write("};\n")
