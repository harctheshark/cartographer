@echo off
:: Change Current Directory to the location of this batch file
CD /D %~dp0

:: Needs Python 3 and d3dx9_31.dll (DirectX End-User Runtime), see the !README.txt next to the script
py -3 "./xlive/Blam/Engine/rasterizer/dx9/pixel_shaders_dx9/preprocessed_hlsl_from_tool/_pixel_shader_compile.py"

pause
