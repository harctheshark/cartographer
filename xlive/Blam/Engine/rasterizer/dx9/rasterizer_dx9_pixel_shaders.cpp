#include "stdafx.h"
#include "rasterizer_dx9_pixel_shaders.h"

#include "cache/cache_files.h"
#include "rasterizer/dx9/pixel_shaders_dx9/preprocessed_hlsl_from_tool/compiled/_compiled_shader_table.h"
#include "shaders/shader_postprocess_definitions.h"
#include "shaders/shader_passes.h"

/* constants */

// The replaced implementations declare this stage as a cube sampler in the stock byte code while the templates bind a 2d bitmap there,
// Turing and newer GPUs then return 0 and the alpha test discards the foliage (see pixel_shaders_dx9\preprocessed_hlsl_from_tool\!README.txt)
#define k_replaced_sampler_stage 4

/* prototypes */

static void rasterizer_dx9_pixel_shaders_replace_implementation_bytecode(const s_pixel_shader_replacement_information* replacement, datum shader_pass_tag_index);

static bool pixel_shader_implementation_declares_cube_sampler(s_shader_pass_postprocess_implementation_new* implementation, uint32 sampler_stage);

static const DWORD* pixel_shader_bytecode_find_sampler_declaration(const DWORD* bytecode, uint32 token_count, uint32 sampler_stage);

/* public code */

void rasterizer_dx9_pixel_shaders_replace_map_bytecode(
	datum shader_pass_tag_index)
{
	const char* tag_name = tag_get_name(shader_pass_tag_index);
	for (uint32 i = 0; i < NUMBEROF(k_pixel_shader_replacement_information); ++i)
	{
		const s_pixel_shader_replacement_information* replacement = &k_pixel_shader_replacement_information[i];
		if (csstricmp(tag_name, replacement->tag_name) == 0)
		{
			rasterizer_dx9_pixel_shaders_replace_implementation_bytecode(replacement, shader_pass_tag_index);
			break;
		}
	}
	return;
}

/* private code */

// Points the compiled shaders of the implementation at the replacement byte code
// Runs before the game creates the Direct3D pixel shaders from this data (scenario_tags_postprocess, inside map_cache_load)
// Only replaces an implementation that still has the stock layout and the stock cube declaration, so passes edited in a custom map are left alone
static void rasterizer_dx9_pixel_shaders_replace_implementation_bytecode(
	const s_pixel_shader_replacement_information* replacement,
	datum shader_pass_tag_index)
{
	s_shader_pass_definition* shader_pass = shader_pass_definition_get(shader_pass_tag_index);
	if (shader_pass->postprocess_definition.count == 0)
	{
		return;
	}

	s_shader_pass_postprocess_definition_new* postprocess = shader_pass->postprocess_definition[0];
	if (postprocess->implementations.count <= replacement->implementation_index)
	{
		return;
	}

	s_shader_pass_postprocess_implementation_new* implementation = postprocess->implementations[replacement->implementation_index];
	if (implementation->pixel_shader.count != replacement->pixel_shader_count
		|| !pixel_shader_implementation_declares_cube_sampler(implementation, k_replaced_sampler_stage))
	{
		return;
	}

	const uintptr_t tag_data_base_address = *tag_data_get();
	for (int32 pixel_shader_index = 0; pixel_shader_index < implementation->pixel_shader.count; ++pixel_shader_index)
	{
		s_pixel_shader* pixel_shader = implementation->pixel_shader[pixel_shader_index];
		const s_compiled_pixel_shader* compiled = &replacement->pixel_shaders[pixel_shader_index];

		static_assert(NUMBEROF(pixel_shader->compiled_shader) == k_compiled_pixel_shader_variant_count);
		for (uint32 variant = 0; variant < k_compiled_pixel_shader_variant_count; ++variant)
		{
			pixel_shader->compiled_shader[variant].data = (int32)((uintptr_t)compiled->bytecode[variant] - tag_data_base_address);
			pixel_shader->compiled_shader[variant].size = compiled->size[variant];
		}
	}
	return;
}

// True when every compiled variant of every pixel shader of the implementation declares the sampler as a cube
static bool pixel_shader_implementation_declares_cube_sampler(
	s_shader_pass_postprocess_implementation_new* implementation,
	uint32 sampler_stage)
{
	for (int32 pixel_shader_index = 0; pixel_shader_index < implementation->pixel_shader.count; ++pixel_shader_index)
	{
		s_pixel_shader* pixel_shader = implementation->pixel_shader[pixel_shader_index];
		for (uint32 variant = 0; variant < NUMBEROF(pixel_shader->compiled_shader); ++variant)
		{
			const tag_data* compiled_shader = &pixel_shader->compiled_shader[variant];
			const DWORD* bytecode = (const DWORD*)tag_data_get_address(compiled_shader);
			if (bytecode == NULL)
			{
				return false;
			}

			const DWORD* usage_token = pixel_shader_bytecode_find_sampler_declaration(bytecode, (uint32)compiled_shader->size / sizeof(DWORD), sampler_stage);
			if (usage_token == NULL || (*usage_token & D3DSP_TEXTURETYPE_MASK) != D3DSTT_CUBE)
			{
				return false;
			}
		}
	}
	return true;
}

// Walks a ps_2_0+ token stream and returns the usage token of the dcl instruction for the sampler register, or NULL
static const DWORD* pixel_shader_bytecode_find_sampler_declaration(
	const DWORD* bytecode,
	uint32 token_count,
	uint32 sampler_stage)
{
	// sampler declarations only exist from ps_2_0 on
	if (token_count < 2 || (bytecode[0] & 0xFFFF0000) != D3DPS_VERSION(0, 0) || D3DSHADER_VERSION_MAJOR(bytecode[0]) < 2)
	{
		return NULL;
	}

	uint32 token = 1;
	while (token < token_count && bytecode[token] != D3DSIO_END)
	{
		const DWORD instruction = bytecode[token];
		const DWORD opcode = instruction & D3DSI_OPCODE_MASK;
		const uint32 length = opcode == D3DSIO_COMMENT ?
			(instruction & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT :
			(instruction & D3DSI_INSTLENGTH_MASK) >> D3DSI_INSTLENGTH_SHIFT;

		if (opcode == D3DSIO_DCL && length == 2 && token + 2 < token_count)
		{
			const DWORD reg = bytecode[token + 2];
			const DWORD register_type = ((reg & D3DSP_REGTYPE_MASK) >> D3DSP_REGTYPE_SHIFT) | ((reg & D3DSP_REGTYPE_MASK2) >> D3DSP_REGTYPE_SHIFT2);
			if (register_type == D3DSPR_SAMPLER && (reg & D3DSP_REGNUM_MASK) == sampler_stage)
			{
				return &bytecode[token + 1];
			}
		}
		token += 1 + length;
	}
	return NULL;
}
