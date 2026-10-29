#include "stdafx.h"
#include "_compiled_shader_table.h"

#include "rasterizer/dx9/pixel_shaders_dx9/preprocessed_hlsl_from_tool/compiled/lightmap_tex_detail_alpha_test_1.h"
#include "rasterizer/dx9/pixel_shaders_dx9/preprocessed_hlsl_from_tool/compiled/lightmap_tex_detail_alpha_test_clamped_1.h"

/* constants */

static const s_compiled_pixel_shader k_lightmap_tex_detail_alpha_test_1_pixel_shaders[4] =
{
	{
		{ k_lightmap_tex_detail_alpha_test_1_0_ps_2_0, k_lightmap_tex_detail_alpha_test_1_0_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_1_0_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_1_0_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_1_0_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_1_0_ps_3_0_mrt) }
	},
	{
		{ k_lightmap_tex_detail_alpha_test_1_1_ps_2_0, k_lightmap_tex_detail_alpha_test_1_1_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_1_1_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_1_1_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_1_1_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_1_1_ps_3_0_mrt) }
	},
	{
		{ k_lightmap_tex_detail_alpha_test_1_2_ps_2_0, k_lightmap_tex_detail_alpha_test_1_2_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_1_2_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_1_2_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_1_2_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_1_2_ps_3_0_mrt) }
	},
	{
		{ k_lightmap_tex_detail_alpha_test_1_3_ps_2_0, k_lightmap_tex_detail_alpha_test_1_3_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_1_3_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_1_3_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_1_3_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_1_3_ps_3_0_mrt) }
	}
};

static const s_compiled_pixel_shader k_lightmap_tex_detail_alpha_test_clamped_1_pixel_shaders[4] =
{
	{
		{ k_lightmap_tex_detail_alpha_test_clamped_1_0_ps_2_0, k_lightmap_tex_detail_alpha_test_clamped_1_0_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_clamped_1_0_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_0_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_0_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_0_ps_3_0_mrt) }
	},
	{
		{ k_lightmap_tex_detail_alpha_test_clamped_1_1_ps_2_0, k_lightmap_tex_detail_alpha_test_clamped_1_1_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_clamped_1_1_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_1_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_1_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_1_ps_3_0_mrt) }
	},
	{
		{ k_lightmap_tex_detail_alpha_test_clamped_1_2_ps_2_0, k_lightmap_tex_detail_alpha_test_clamped_1_2_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_clamped_1_2_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_2_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_2_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_2_ps_3_0_mrt) }
	},
	{
		{ k_lightmap_tex_detail_alpha_test_clamped_1_3_ps_2_0, k_lightmap_tex_detail_alpha_test_clamped_1_3_ps_2_0_zprime, k_lightmap_tex_detail_alpha_test_clamped_1_3_ps_3_0_mrt },
		{ sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_3_ps_2_0), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_3_ps_2_0_zprime), sizeof(k_lightmap_tex_detail_alpha_test_clamped_1_3_ps_3_0_mrt) }
	}
};

const s_pixel_shader_replacement_information k_pixel_shader_replacement_information[2] =
{
	{
		"shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test",
		1,
		NUMBEROF(k_lightmap_tex_detail_alpha_test_1_pixel_shaders),
		k_lightmap_tex_detail_alpha_test_1_pixel_shaders
	},
	{
		"shaders\\shader_passes\\lightmap\\lightmap_tex_detail_alpha_test_clamped",
		1,
		NUMBEROF(k_lightmap_tex_detail_alpha_test_clamped_1_pixel_shaders),
		k_lightmap_tex_detail_alpha_test_clamped_1_pixel_shaders
	}
};
