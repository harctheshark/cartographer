This folder holds pixel shaders of shader pass tags that we replace in the map files, the same way vertex_shaders_dx9 does for vertex shaders.
The sources are the pre-processed pixel shaders the Halo 2 Vista tool writes to tags\rasterizer\pixel_shaders_dx9 (one #case per pixel shader of the implementation).

Compiling the unmodified sources with the compiler the tool uses (d3dx9_31.dll D3DXCompileShader, flags 0, ps_2_0, ps_2_0 with ZPRIME, ps_3_0 with MRT)
reproduces the byte code in the stock maps exactly, so the replacements differ from the stock shaders only by the fix below.
Newer compilers (bin\fxc.exe) produce different code for these shaders, so they are not used here.

shaders_shader_passes_lightmap_lightmap_tex_detail_alpha_test_1.psh
shaders_shader_passes_lightmap_lightmap_tex_detail_alpha_test_clamped_1.psh
	Implementation 1 (per-vertex lightmap) of shaders\shader_passes\lightmap\lightmap_tex_detail_alpha_test(_clamped).
	The tool source (.txt) declares texture 4 with mode=2 (cube) while the vertex shader only writes oT4.rg and implementations 0, 2 and 3 use mode=0 (2d),
	so the stock shaders sample the 2d bitmap the templates bind there through a cube sampler. Turing and newer GPUs return 0 and the foliage is invisible.
	Fixed by changing in every #case what mode=0 generates (as in implementation 0):
		samplerCUBE s4 : register(S4);  ->  sampler2D s4 : register(S4);
		texCUBE(s4, tc4)                ->  tex2Dproj(s4, tc4)

To rebuild compiled\ run "build dx9 pixel shaders.bat" in the root of the repository (needs Python 3 and d3dx9_31.dll from the DirectX End-User Runtime).
