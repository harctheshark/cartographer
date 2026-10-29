#pragma once

/* enums */

// same order as s_pixel_shader::compiled_shader
enum e_compiled_pixel_shader_variant
{
	_compiled_pixel_shader_ps_2_0,
	_compiled_pixel_shader_ps_2_0_zprime,
	_compiled_pixel_shader_ps_3_0_mrt,

	k_compiled_pixel_shader_variant_count
};

/* structures */

struct s_compiled_pixel_shader
{
	const unsigned char* bytecode[k_compiled_pixel_shader_variant_count];
	int16 size[k_compiled_pixel_shader_variant_count];
};

struct s_pixel_shader_replacement_information
{
	const char* tag_name;
	int32 implementation_index;
	int32 pixel_shader_count;
	const s_compiled_pixel_shader* pixel_shaders;
};

/* constants */

extern const s_pixel_shader_replacement_information k_pixel_shader_replacement_information[2];
